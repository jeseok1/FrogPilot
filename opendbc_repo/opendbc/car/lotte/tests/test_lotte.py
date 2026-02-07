import math
import pytest

from opendbc.can import CANPacker, CANParser
from opendbc.car.lotte.values import (
  CAR, DBC, ACCEL_TO_TORQUE_KF, RPM_TO_MS, MASS, GRAVITY,
  GEAR_RATIO, TIRE_RADIUS, MAX_TORQUE, V_EGO_STARTING,
  STARTING_TORQUE_PCT, BRAKE_PRESSURE_GAIN, MAX_BRAKE_PRESSURE,
  MAX_STEER_ANGLE, AUTOWARE_TIMEOUT, ACCEL_PID_OUTPUT_LIMIT,
)
from opendbc.car.lotte import lottecan


# ---------- Constants ----------

class TestConstants:
  def test_rpm_to_ms_formula(self):
    expected = 2.0 * math.pi * TIRE_RADIUS / (GEAR_RATIO * 60.0)
    assert abs(RPM_TO_MS - expected) < 1e-8

  def test_rpm_to_ms_value(self):
    # 1000 RPM should be roughly 2.1 m/s
    v = 1000.0 * RPM_TO_MS
    assert 1.5 < v < 3.0

  def test_accel_to_torque_kf(self):
    expected = MASS * TIRE_RADIUS / (GEAR_RATIO * MAX_TORQUE) * 100.0
    assert abs(ACCEL_TO_TORQUE_KF - expected) < 1e-6

  def test_kf_reasonable_range(self):
    # 1 m/s^2 should need roughly 30-40% torque
    assert 25 < ACCEL_TO_TORQUE_KF < 45

  def test_max_speed_reachable(self):
    # 40 km/h = 11.11 m/s, check that some RPM maps there
    max_speed_ms = 11.11
    rpm_needed = max_speed_ms / RPM_TO_MS
    assert 4000 < rpm_needed < 7000

  def test_platform_config(self):
    platform = CAR.LOTTE_SHUTTLE
    assert platform.config.specs.mass == MASS
    assert platform.config.specs.wheelbase == 3.0
    assert platform.config.specs.steerRatio == 1.0

  def test_dbc_map(self):
    from opendbc.car import Bus
    dbc = DBC[CAR.LOTTE_SHUTTLE]
    assert 'lotte_shuttle_chassis' in dbc[Bus.main]
    assert 'lotte_sensor' in dbc[Bus.adas]


# ---------- CAN Encoding (lottecan.py) ----------

class TestLotteCan:
  @pytest.fixture
  def packer(self):
    return CANPacker('lotte_shuttle_chassis')

  def test_motor_command_enabled(self, packer):
    addr, dat, bus = lottecan.create_motor_command(packer, torque_pct=50.0, direction=1, enable=True)
    assert addr == 353  # 0x161
    assert bus == 0
    assert len(dat) == 8

  def test_motor_command_disabled(self, packer):
    addr, dat, bus = lottecan.create_motor_command(packer, torque_pct=0.0, direction=0, enable=False)
    assert addr == 353
    # throttleOpenDegree should be 0 when disabled
    parser = CANParser('lotte_shuttle_chassis', [('Motor3ReqMsg', 0)], 0)
    parser.update([(0, [(addr, dat, bus)])])
    assert parser.vl['Motor3ReqMsg']['throttleOpenDegree'] == 0
    assert parser.vl['Motor3ReqMsg']['controlModel'] == 0

  def test_motor_command_torque_roundtrip(self, packer):
    parser = CANParser('lotte_shuttle_chassis', [('Motor3ReqMsg', 0)], 0)
    for torque in [0, 25, 50, 75, 100]:
      addr, dat, bus = lottecan.create_motor_command(packer, torque, 1, True)
      parser.update([(0, [(addr, dat, bus)])])
      assert parser.vl['Motor3ReqMsg']['throttleOpenDegree'] == torque

  def test_front_steering_command(self, packer):
    addr, dat, bus = lottecan.create_front_steering_command(packer, 10.5, 10.0, 42, True)
    assert addr == 355  # 0x163
    assert bus == 0
    parser = CANParser('lotte_shuttle_chassis', [('frontSteeringReq', 0)], 0)
    parser.update([(0, [(addr, dat, bus)])])
    assert abs(parser.vl['frontSteeringReq']['angle'] - 10.5) < 0.02  # 0.01 resolution
    assert parser.vl['frontSteeringReq']['mode'] == 1

  def test_rear_steering_command(self, packer):
    addr, dat, bus = lottecan.create_rear_steering_command(packer, -5.0, 10.0, 7, True)
    assert addr == 356  # 0x164
    parser = CANParser('lotte_shuttle_chassis', [('rearSteeringReq', 0)], 0)
    parser.update([(0, [(addr, dat, bus)])])
    assert abs(parser.vl['rearSteeringReq']['angle'] - (-5.0)) < 0.02

  def test_steering_negative_angle(self, packer):
    parser = CANParser('lotte_shuttle_chassis', [('frontSteeringReq', 0)], 0)
    addr, dat, bus = lottecan.create_front_steering_command(packer, -19.05, 10.0, 0, True)
    parser.update([(0, [(addr, dat, bus)])])
    assert abs(parser.vl['frontSteeringReq']['angle'] - (-19.05)) < 0.02

  def test_brake_command(self, packer):
    addr, dat, bus = lottecan.create_brake_command(packer, 50.0, 3, True)
    assert addr == 336  # 0x150
    parser = CANParser('lotte_shuttle_chassis', [('EHBReqMsg', 0)], 0)
    parser.update([(0, [(addr, dat, bus)])])
    assert parser.vl['EHBReqMsg']['pressure'] == 50
    assert parser.vl['EHBReqMsg']['enable'] == 1

  def test_brake_command_disabled(self, packer):
    addr, dat, bus = lottecan.create_brake_command(packer, 50.0, 0, False)
    parser = CANParser('lotte_shuttle_chassis', [('EHBReqMsg', 0)], 0)
    parser.update([(0, [(addr, dat, bus)])])
    assert parser.vl['EHBReqMsg']['pressure'] == 0
    assert parser.vl['EHBReqMsg']['enable'] == 0

  def test_brake_counter_wraps(self, packer):
    # counter is 4-bit (0-15)
    _addr1, _dat1, _bus1 = lottecan.create_brake_command(packer, 10.0, 15, True)
    addr2, dat2, bus2 = lottecan.create_brake_command(packer, 10.0, 16, True)
    # counter 16 & 0xF = 0
    parser = CANParser('lotte_shuttle_chassis', [('EHBReqMsg', 0)], 0)
    parser.update([(0, [(addr2, dat2, bus2)])])
    assert parser.vl['EHBReqMsg']['counter'] == 0

  def test_epb_engage(self, packer):
    addr, dat, bus = lottecan.create_epb_command(packer, engage=True)
    assert addr == 688  # 0x2B0
    parser = CANParser('lotte_shuttle_chassis', [('EPBReqMsg', 0)], 0)
    parser.update([(0, [(addr, dat, bus)])])
    assert parser.vl['EPBReqMsg']['request'] == 1

  def test_epb_release(self, packer):
    addr, dat, bus = lottecan.create_epb_command(packer, engage=False)
    parser = CANParser('lotte_shuttle_chassis', [('EPBReqMsg', 0)], 0)
    parser.update([(0, [(addr, dat, bus)])])
    assert parser.vl['EPBReqMsg']['request'] == 0


# ---------- Control Logic ----------

class TestControlLogic:
  def test_feedforward_zero_accel(self):
    torque = 0.0 * ACCEL_TO_TORQUE_KF
    assert torque == 0.0

  def test_feedforward_1ms2(self):
    torque = 1.0 * ACCEL_TO_TORQUE_KF
    assert 25 < torque < 45  # ~33.4%

  def test_feedforward_max_accel(self):
    torque = 2.0 * ACCEL_TO_TORQUE_KF
    assert torque < 100  # should not exceed 100% at max allowed accel

  def test_gravity_comp_flat(self):
    pitch_rad = 0.0
    gravity_comp = MASS * GRAVITY * math.sin(pitch_rad) * TIRE_RADIUS / (GEAR_RATIO * MAX_TORQUE) * 100.0
    assert gravity_comp == 0.0

  def test_gravity_comp_uphill(self):
    # 5 degree uphill
    pitch_rad = math.radians(5.0)
    gravity_comp = MASS * GRAVITY * math.sin(pitch_rad) * TIRE_RADIUS / (GEAR_RATIO * MAX_TORQUE) * 100.0
    # Should add positive torque to overcome gravity
    assert gravity_comp > 0
    assert 20.0 < gravity_comp < 35.0  # ~28.6% for 2500kg shuttle at 5 deg

  def test_gravity_comp_downhill(self):
    # -5 degree downhill
    pitch_rad = math.radians(-5.0)
    gravity_comp = MASS * GRAVITY * math.sin(pitch_rad) * TIRE_RADIUS / (GEAR_RATIO * MAX_TORQUE) * 100.0
    assert gravity_comp < 0

  def test_starting_torque_guarantee(self):
    # When accel > 0 and vEgo < V_EGO_STARTING, torque should be at least STARTING_TORQUE_PCT
    accel = 0.1  # very small accel request
    torque_ff = accel * ACCEL_TO_TORQUE_KF  # ~3.3%
    assert torque_ff < STARTING_TORQUE_PCT
    torque = max(torque_ff, STARTING_TORQUE_PCT)
    assert torque == STARTING_TORQUE_PCT

  def test_starting_torque_not_applied_at_speed(self):
    # When vEgo >= V_EGO_STARTING, starting torque should NOT override
    accel = 0.5
    torque_ff = accel * ACCEL_TO_TORQUE_KF  # ~16.7%
    vEgo = 1.0  # above V_EGO_STARTING
    if accel > 0 and vEgo < V_EGO_STARTING:
      torque = max(torque_ff, STARTING_TORQUE_PCT)
    else:
      torque = torque_ff
    assert torque == torque_ff  # no override

  def test_brake_pressure_from_decel(self):
    decel = -2.0  # m/s^2
    pressure = min(-decel * BRAKE_PRESSURE_GAIN, MAX_BRAKE_PRESSURE)
    assert pressure == 40.0  # 2.0 * 20.0

  def test_brake_pressure_clamp(self):
    decel = -10.0
    pressure = min(-decel * BRAKE_PRESSURE_GAIN, MAX_BRAKE_PRESSURE)
    assert pressure == MAX_BRAKE_PRESSURE

  def test_steer_clamp(self):
    import numpy as np
    for angle in [-30.0, -19.05, 0.0, 19.05, 30.0]:
      clamped = float(np.clip(angle, -MAX_STEER_ANGLE, MAX_STEER_ANGLE))
      assert -MAX_STEER_ANGLE <= clamped <= MAX_STEER_ANGLE

  def test_pid_output_limit(self):
    # PID should not exceed +/-ACCEL_PID_OUTPUT_LIMIT
    assert ACCEL_PID_OUTPUT_LIMIT == 20.0


# ---------- Sensor DBC Encoding/Decoding ----------

class TestSensorDBC:
  @pytest.fixture
  def packer(self):
    return CANPacker('lotte_sensor')

  @pytest.fixture
  def parser_imu(self):
    messages = [
      ('StatusWord', 0),
      ('EulerAngles', 0),
      ('RateOfTurn', 0),
      ('Acceleration', 0),
    ]
    return CANParser('lotte_sensor', messages, 1)

  @pytest.fixture
  def parser_gps(self):
    messages = [
      ('GnssHeadingSpeed', 0),
      ('GnssAccuracy', 0),
      ('GnssVrsStatus', 0),
    ]
    return CANParser('lotte_sensor', messages, 1)

  @pytest.fixture
  def parser_autoware(self):
    return CANParser('lotte_sensor', [('AutowareCommand', 0)], 1)

  def test_autoware_command_roundtrip(self, packer, parser_autoware):
    values = {
      'target_accel': 1.5,
      'target_steer_angle': -10.0,
      'enabled': 1,
      'counter': 42,
    }
    addr, dat, bus = packer.make_can_msg('AutowareCommand', 1, values)
    assert addr == 512  # 0x200
    parser_autoware.update([(1, [(addr, dat, bus)])])
    assert abs(parser_autoware.vl['AutowareCommand']['target_accel'] - 1.5) < 0.002
    assert abs(parser_autoware.vl['AutowareCommand']['target_steer_angle'] - (-10.0)) < 0.02
    assert parser_autoware.vl['AutowareCommand']['enabled'] == 1
    assert parser_autoware.vl['AutowareCommand']['counter'] == 42

  def test_autoware_negative_accel(self, packer, parser_autoware):
    values = {
      'target_accel': -3.0,
      'target_steer_angle': 0.0,
      'enabled': 1,
      'counter': 0,
    }
    addr, dat, bus = packer.make_can_msg('AutowareCommand', 1, values)
    parser_autoware.update([(1, [(addr, dat, bus)])])
    assert abs(parser_autoware.vl['AutowareCommand']['target_accel'] - (-3.0)) < 0.002

  def test_gps_heading_speed_roundtrip(self, packer, parser_gps):
    values = {
      'heading': 180.0,
      'speed': 5.555,
      'fixType': 5,
      'numSat': 20,
    }
    addr, dat, bus = packer.make_can_msg('GnssHeadingSpeed', 1, values)
    assert addr == 258  # 0x102
    parser_gps.update([(1, [(addr, dat, bus)])])
    assert abs(parser_gps.vl['GnssHeadingSpeed']['heading'] - 180.0) < 0.02
    assert abs(parser_gps.vl['GnssHeadingSpeed']['speed'] - 5.555) < 0.002
    assert parser_gps.vl['GnssHeadingSpeed']['fixType'] == 5
    assert parser_gps.vl['GnssHeadingSpeed']['numSat'] == 20

  def test_vrs_status_roundtrip(self, packer, parser_gps):
    values = {
      'ntripConnected': 1,
      'diffAge': 1.5,
      'rtcmMsgRate': 10,
    }
    addr, dat, bus = packer.make_can_msg('GnssVrsStatus', 1, values)
    parser_gps.update([(1, [(addr, dat, bus)])])
    assert parser_gps.vl['GnssVrsStatus']['ntripConnected'] == 1
    assert abs(parser_gps.vl['GnssVrsStatus']['diffAge'] - 1.5) < 0.02

  def test_imu_acceleration_roundtrip(self, packer, parser_imu):
    values = {'accX': 2.5, 'accY': -0.5, 'accZ': 9.81}
    addr, dat, bus = packer.make_can_msg('Acceleration', 1, values)
    assert addr == 52  # 0x34
    parser_imu.update([(1, [(addr, dat, bus)])])
    assert abs(parser_imu.vl['Acceleration']['accX'] - 2.5) < 0.01
    assert abs(parser_imu.vl['Acceleration']['accY'] - (-0.5)) < 0.01

  def test_euler_angles_roundtrip(self, packer, parser_imu):
    values = {'roll': 1.0, 'pitch': -3.5, 'yaw': 90.0}
    addr, dat, bus = packer.make_can_msg('EulerAngles', 1, values)
    parser_imu.update([(1, [(addr, dat, bus)])])
    assert abs(parser_imu.vl['EulerAngles']['pitch'] - (-3.5)) < 0.01
    assert abs(parser_imu.vl['EulerAngles']['yaw'] - 90.0) < 0.01

  def test_rate_of_turn_roundtrip(self, packer, parser_imu):
    values = {'gyrX': 0.0, 'gyrY': 0.0, 'gyrZ': 0.15}
    addr, dat, bus = packer.make_can_msg('RateOfTurn', 1, values)
    parser_imu.update([(1, [(addr, dat, bus)])])
    assert abs(parser_imu.vl['RateOfTurn']['gyrZ'] - 0.15) < 0.005


# ---------- Autoware Switching Logic ----------

class TestAutowareSwitching:
  def test_autoware_alive_detection(self):
    import time
    last_counter = -1
    last_time = 0.0

    # Simulate counter change
    new_counter = 5
    now = time.monotonic()
    if new_counter != last_counter:
      last_counter = new_counter
      last_time = now

    alive = (now - last_time) < AUTOWARE_TIMEOUT
    assert alive is True

  def test_autoware_timeout(self):
    import time
    # Simulate stale data
    last_time = time.monotonic() - 0.2  # 200ms ago
    now = time.monotonic()
    alive = (now - last_time) < AUTOWARE_TIMEOUT
    assert alive is False

  def test_autoware_counter_no_change(self):
    import time
    last_counter = 10
    last_time = time.monotonic() - 0.05  # 50ms ago

    # Same counter received
    new_counter = 10
    now = time.monotonic()
    if new_counter != last_counter:
      last_time = now
    # last_time not updated, but still within timeout
    alive = (now - last_time) < AUTOWARE_TIMEOUT
    assert alive is True  # 50ms < 100ms timeout

  def test_timeout_constant(self):
    assert AUTOWARE_TIMEOUT == 0.1
