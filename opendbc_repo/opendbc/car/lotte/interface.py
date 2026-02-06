import math
from opendbc.car import get_safety_config, structs
from opendbc.car.lotte.carcontroller import CarController
from opendbc.car.lotte.carstate import CarState
from opendbc.car.interfaces import CarInterfaceBase


class CarInterface(CarInterfaceBase):
  CarState = CarState
  CarController = CarController

  @staticmethod
  def _get_params(ret: structs.CarParams, candidate, fingerprint, car_fw, alpha_long, is_release, docs) -> structs.CarParams:
    ret.brand = "lotte"
    ret.safetyConfigs = [get_safety_config(structs.CarParams.SafetyModel.allOutput)]

    # Lateral
    ret.steerControlType = structs.CarParams.SteerControlType.angle
    ret.minSteerSpeed = -math.inf  # can steer at any speed
    ret.steerActuatorDelay = 0.1
    ret.steerLimitTimer = 1.0
    ret.maxLateralAccel = math.inf

    # Longitudinal
    ret.openpilotLongitudinalControl = True
    ret.pcmCruise = False
    ret.vEgoStarting = 0.3
    ret.stopAccel = -2.0
    ret.stoppingDecelRate = 0.8
    ret.vEgoStopping = 0.3
    ret.longitudinalActuatorDelay = 0.1

    # No radar
    ret.radarUnavailable = True

    # No wheel speeds (only motor RPM)
    ret.wheelSpeedFactor = 1.0

    return ret

  @staticmethod
  def get_pid_accel_limits(CP, current_speed, cruise_speed):
    return -3.0, 2.0
