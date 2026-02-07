import math
import time

from cereal import custom
from opendbc.can import CANParser
from opendbc.car import Bus, DT_CTRL, structs
from opendbc.car.interfaces import CarStateBase
from opendbc.car.lotte.values import DBC, RPM_TO_MS, AUTOWARE_TIMEOUT, IMU_OFFSET_X


class CarState(CarStateBase):
  def __init__(self, CP, FPCP):
    super().__init__(CP, FPCP)

    # IMU data (offset-compensated)
    self.imu_accel_x = 0.0
    self.imu_accel_y = 0.0
    self.imu_pitch = 0.0
    self.imu_yaw_rate = 0.0
    self.imu_valid = False
    self._prev_yaw_rate = 0.0

    # GPS data
    self.gps_speed = 0.0
    self.gps_heading = 0.0
    self.gps_fix_type = 0
    self.gps_h_acc = 999.0
    self.vrs_connected = False
    self.vrs_diff_age = 999.0

    # Autoware command
    self.autoware_accel = 0.0
    self.autoware_steer_angle = 0.0
    self.autoware_enabled = False
    self.autoware_alive = False
    self._autoware_last_counter = -1
    self._autoware_last_time = 0.0

    # Motor state
    self.motor_error_code = 0
    self.motor_fault_level = 0
    self.motor_temp = 0
    self.cont_temp = 0

  def update(self, can_parsers, frogpilot_toggles) -> structs.CarState:
    cp_chassis = can_parsers[Bus.main]
    cp_sensor = can_parsers[Bus.adas]

    ret = structs.CarState()

    # --- Bus 0: Chassis CAN ---

    # Speed from motor RPM
    motor_rpm = cp_chassis.vl['Motor3Status1']['currentSpeed']
    ret.vEgoRaw = abs(motor_rpm) * RPM_TO_MS
    ret.vEgo, ret.aEgo = self.update_speed_kf(ret.vEgoRaw)
    ret.standstill = ret.vEgo < 0.01

    # Steering angle (front)
    ret.steeringAngleDeg = cp_chassis.vl['frontSteeringStatus']['angle']

    # Brake
    brake_pressure = cp_chassis.vl['EHBStatusMsg']['pressure']
    ret.brake = brake_pressure / 100.0  # normalize to 0-1 range (100 bar max)
    ret.brakePressed = brake_pressure > 2.0

    # Battery
    ret.fuelGauge = cp_chassis.vl['BMSStatusStdMsg']['soc'] / 100.0

    # Motor status
    self.motor_error_code = cp_chassis.vl['Motor3Status1']['errorCode']
    self.motor_fault_level = cp_chassis.vl['Motor3Status1']['faultLevel']
    self.motor_temp = cp_chassis.vl['Motor3Status1']['motorTemp']
    self.cont_temp = cp_chassis.vl['Motor3Status1']['contTemp']

    # Steering fault
    front_sensor_failed = cp_chassis.vl['frontSteeringStatus']['sensorFailed']
    rear_sensor_failed = cp_chassis.vl['rearSteeringStatus']['sensorFailed']
    ret.steerFaultPermanent = bool(front_sensor_failed or rear_sensor_failed)
    ret.steerFaultTemporary = False

    # EPB status
    epb_status = cp_chassis.vl['EPB1Status']['status']

    # Gear: shuttle is always in drive unless EPB is engaged
    if epb_status > 0:
      ret.gearShifter = structs.CarState.GearShifter.park
    else:
      ret.gearShifter = structs.CarState.GearShifter.drive

    # Cruise state: shuttle has no stock cruise, always enabled when system is on
    ret.cruiseState.available = True
    ret.cruiseState.enabled = True
    ret.cruiseState.speed = 0.0

    # --- Bus 1: Sensor CAN ---

    # Xsens IMU (raw readings)
    raw_accel_x = cp_sensor.vl['Acceleration']['accX']
    raw_accel_y = cp_sensor.vl['Acceleration']['accY']
    self.imu_pitch = cp_sensor.vl['EulerAngles']['pitch']
    raw_yaw_rate = cp_sensor.vl['RateOfTurn']['gyrZ']
    self.imu_valid = bool(cp_sensor.vl['StatusWord']['SelfTestOk'] and
                          cp_sensor.vl['StatusWord']['OrientationValid'])

    # IMU offset compensation: IMU is mounted at IMU_OFFSET_X from CG
    # Rigid body: a_IMU = a_CG + α×r + ω×(ω×r)
    #   accX_CG = accX_IMU + ωz² · IMU_OFFSET_X  (centripetal)
    #   accY_CG = accY_IMU - αz  · IMU_OFFSET_X  (Euler)
    if self.imu_valid:
      centripetal_x = raw_yaw_rate ** 2 * IMU_OFFSET_X
      yaw_accel = (raw_yaw_rate - self._prev_yaw_rate) / DT_CTRL
      euler_y = yaw_accel * IMU_OFFSET_X
      self.imu_accel_x = raw_accel_x + centripetal_x
      self.imu_accel_y = raw_accel_y - euler_y
    else:
      self.imu_accel_x = raw_accel_x
      self.imu_accel_y = raw_accel_y

    self._prev_yaw_rate = raw_yaw_rate
    self.imu_yaw_rate = raw_yaw_rate
    ret.yawRate = self.imu_yaw_rate  # rad/s (openpilot expects rad/s)

    # GPS
    self.gps_speed = cp_sensor.vl['GnssHeadingSpeed']['speed']
    self.gps_heading = cp_sensor.vl['GnssHeadingSpeed']['heading']
    self.gps_fix_type = int(cp_sensor.vl['GnssHeadingSpeed']['fixType'])
    self.gps_h_acc = cp_sensor.vl['GnssAccuracy']['hAcc']

    # VRS status
    self.vrs_connected = bool(cp_sensor.vl['GnssVrsStatus']['ntripConnected'])
    self.vrs_diff_age = cp_sensor.vl['GnssVrsStatus']['diffAge']

    # Autoware command
    autoware_counter = int(cp_sensor.vl['AutowareCommand']['counter'])
    now = time.monotonic()

    if autoware_counter != self._autoware_last_counter:
      self._autoware_last_counter = autoware_counter
      self._autoware_last_time = now

    self.autoware_alive = (now - self._autoware_last_time) < AUTOWARE_TIMEOUT
    self.autoware_enabled = bool(cp_sensor.vl['AutowareCommand']['enabled'])
    self.autoware_accel = cp_sensor.vl['AutowareCommand']['target_accel']
    self.autoware_steer_angle = cp_sensor.vl['AutowareCommand']['target_steer_angle']

    # FrogPilot variables
    fp_ret = custom.FrogPilotCarState.new_message()

    return ret, fp_ret

  @staticmethod
  def get_can_parsers(CP):
    chassis_messages = [
      ('Motor3Status1', 50),
      ('Motor3Status2', 50),
      ('frontSteeringStatus', 50),
      ('rearSteeringStatus', 50),
      ('EHBStatusMsg', 50),
      ('BMSStatusStdMsg', 10),
      ('EPB1Status', 10),
    ]

    sensor_messages = [
      ('StatusWord', 100),
      ('EulerAngles', 100),
      ('RateOfTurn', 100),
      ('Acceleration', 100),
      ('FreeAcceleration', 100),
      ('GnssHeadingSpeed', 10),
      ('GnssAccuracy', 10),
      ('GnssVrsStatus', 1),
      ('AutowareCommand', 100),
    ]

    return {
      Bus.main: CANParser(DBC[CP.carFingerprint][Bus.main], chassis_messages, 0),
      Bus.adas: CANParser(DBC[CP.carFingerprint][Bus.adas], sensor_messages, 1),
    }
