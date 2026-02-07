from opendbc.car import Bus, CarSpecs, PlatformConfig, Platforms
from opendbc.car.structs import CarParams
from opendbc.car.docs_definitions import CarDocs

# Physical constants
GEAR_RATIO = 19.2
TIRE_RADIUS = 0.385  # m
MAX_TORQUE = 150.0  # Nm
MAX_SPEED = 11.11  # m/s (40 km/h)
MAX_STEER_ANGLE = 19.05  # deg
RPM_TO_MS = 2.0 * 3.14159265 * TIRE_RADIUS / (GEAR_RATIO * 60.0)  # ~0.002104

# Control constants
V_EGO_STARTING = 0.3  # m/s
STARTING_TORQUE_PCT = 25.0  # %
ACCEL_TO_TORQUE_KF = 2500.0 * TIRE_RADIUS / (GEAR_RATIO * MAX_TORQUE) * 100.0  # ~33.4 %/(m/s^2)
BRAKE_PRESSURE_GAIN = 20.0  # bar/(m/s^2), tuning required
MAX_BRAKE_PRESSURE = 100.0  # bar

# Accel PID initial gains
ACCEL_PID_KP = 5.0
ACCEL_PID_KI = 1.0
ACCEL_PID_KD = 0.0
ACCEL_PID_OUTPUT_LIMIT = 20.0  # % torque

# Gravity compensation
MASS = 2500.0  # kg (estimated)
GRAVITY = 9.81  # m/s^2

# Autoware command
AUTOWARE_TIMEOUT = 0.1  # seconds


class CarControllerParams:
  def __init__(self, CP):
    pass


class CAR(Platforms):
  LOTTE_SHUTTLE = PlatformConfig(
    [CarDocs("Lotte Shuttle", package="All")],
    CarSpecs(mass=MASS, wheelbase=3.0, steerRatio=1.0, centerToFrontRatio=0.5),
    {Bus.main: 'lotte_shuttle_chassis', Bus.adas: 'lotte_sensor'},
  )


DBC = CAR.create_dbc_map()
