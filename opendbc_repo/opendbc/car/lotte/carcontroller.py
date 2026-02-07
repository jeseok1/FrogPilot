import math
import numpy as np

from opendbc.can import CANPacker
from opendbc.car import Bus, DT_CTRL
from opendbc.car.common.pid import PIDController
from opendbc.car.lotte import lottecan
from opendbc.car.lotte.values import (
  ACCEL_TO_TORQUE_KF, ACCEL_PID_KP, ACCEL_PID_KI, ACCEL_PID_KD,
  ACCEL_PID_OUTPUT_LIMIT, BRAKE_PRESSURE_GAIN, MAX_BRAKE_PRESSURE,
  MAX_STEER_ANGLE, MAX_TORQUE_PCT, STARTING_TORQUE_PCT, STARTING_FADE_END,
  MASS, GRAVITY, GEAR_RATIO, TIRE_RADIUS, MAX_TORQUE,
)
from opendbc.car.interfaces import CarControllerBase

STEER_SPEED_DEG_S = 10.0  # steering actuation speed (deg/s)


class CarController(CarControllerBase):
  def __init__(self, dbc_names, CP):
    super().__init__(dbc_names, CP)
    self.packer = CANPacker(dbc_names[Bus.main])

    # Local acceleration PID (cascade inner loop)
    self.accel_pid = PIDController(
      k_p=ACCEL_PID_KP,
      k_i=ACCEL_PID_KI,
      k_d=ACCEL_PID_KD,
      pos_limit=ACCEL_PID_OUTPUT_LIMIT,
      neg_limit=-ACCEL_PID_OUTPUT_LIMIT,
      rate=1.0 / DT_CTRL,
    )

    # Counters
    self.steer_counter = 0
    self.brake_counter = 0

    # Switching state
    self._was_autoware = False

  def update(self, CC, CS, now_nanos, frogpilot_toggles):
    can_sends = []
    actuators = CC.actuators

    # Determine control source: Autoware CAN vs FrogPilot actuators
    use_autoware = CS.autoware_alive and CS.autoware_enabled

    # Safe transition: if switching from Autoware to FrogPilot, brief brake
    if self._was_autoware and not use_autoware and CC.enabled:
      target_accel = 0.0
      target_steer = CS.out.steeringAngleDeg  # hold current angle
    elif use_autoware:
      target_accel = CS.autoware_accel
      target_steer = CS.autoware_steer_angle
    else:
      target_accel = float(actuators.accel)
      target_steer = float(actuators.steeringAngleDeg)

    self._was_autoware = use_autoware

    # --- Longitudinal Control ---
    torque_pct = 0.0
    brake_pressure = 0.0

    if CC.longActive:
      if target_accel >= 0:
        # 1) Feedforward
        torque_ff = target_accel * ACCEL_TO_TORQUE_KF

        # 2) Gravity compensation (pitch from Xsens IMU)
        pitch_rad = math.radians(CS.imu_pitch) if CS.imu_valid else 0.0
        gravity_comp = MASS * GRAVITY * math.sin(pitch_rad) * TIRE_RADIUS / (GEAR_RATIO * MAX_TORQUE) * 100.0

        # 3) Local acceleration PID (Xsens accX feedback)
        accel_error = target_accel - CS.imu_accel_x if CS.imu_valid else 0.0
        torque_fb = self.accel_pid.update(accel_error)

        # 4) Combine
        torque_pct = torque_ff + gravity_comp + torque_fb

        # 5) Starting torque guarantee (smooth fade-out to avoid torque dip)
        #    Full floor at vEgo=0, linearly fades to 0 at vEgo=STARTING_FADE_END
        #    so PID integrator has time to wind up before floor is fully released
        if target_accel > 0 and CS.out.vEgo < STARTING_FADE_END:
          fade = max(0.0, 1.0 - CS.out.vEgo / STARTING_FADE_END)
          starting_floor = STARTING_TORQUE_PCT * fade
          torque_pct = max(torque_pct, starting_floor)

        torque_pct = float(np.clip(torque_pct, 0, MAX_TORQUE_PCT))
      else:
        # Braking
        brake_pressure = float(np.clip(-target_accel * BRAKE_PRESSURE_GAIN, 0, MAX_BRAKE_PRESSURE))
        self.accel_pid.reset()
    else:
      self.accel_pid.reset()

    # --- Lateral Control ---
    steer_angle = 0.0
    if CC.latActive:
      steer_angle = float(np.clip(target_steer, -MAX_STEER_ANGLE, MAX_STEER_ANGLE))

    # --- Build CAN messages ---

    # Motor command (send every frame)
    direction = 1 if torque_pct > 0 else 0  # 1=forward, 0=stop
    can_sends.append(lottecan.create_motor_command(
      self.packer, torque_pct, direction, CC.longActive and target_accel >= 0))

    # Steering commands (send every frame, front + rear identical)
    can_sends.append(lottecan.create_front_steering_command(
      self.packer, steer_angle, STEER_SPEED_DEG_S, self.steer_counter, CC.latActive))
    can_sends.append(lottecan.create_rear_steering_command(
      self.packer, steer_angle, STEER_SPEED_DEG_S, self.steer_counter, CC.latActive))

    # Brake command (send every frame)
    can_sends.append(lottecan.create_brake_command(
      self.packer, brake_pressure, self.brake_counter, CC.longActive and target_accel < 0))

    # Update counters
    self.steer_counter = (self.steer_counter + 1) % 256
    self.brake_counter = (self.brake_counter + 1) % 16

    # Update actuators output
    new_actuators = actuators.as_builder()
    new_actuators.steeringAngleDeg = steer_angle
    new_actuators.accel = target_accel

    self.frame += 1
    return new_actuators, can_sends
