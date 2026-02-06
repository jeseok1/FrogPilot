def create_motor_command(packer, torque_pct, direction, enable):
  """Create Motor3ReqMsg (0x161).

  Args:
    packer: CANPacker instance
    torque_pct: motor torque percentage (0-100)
    direction: 0=stop, 1=forward, 2=reverse
    enable: whether motor control is enabled
  """
  values = {
    'controlModel': 1 if enable else 0,  # 1=torque control
    'direction': direction,
    'mode': 1 if enable else 0,
    'speed': 0,
    'torque': 0,
    'throttleOpenDegree': int(round(torque_pct)) if enable else 0,
    'count': 0,  # counter managed externally if needed
  }
  return packer.make_can_msg('Motor3ReqMsg', 0, values)


def create_front_steering_command(packer, angle_deg, speed_deg_s, counter, enable):
  """Create frontSteeringReq (0x163).

  Args:
    packer: CANPacker instance
    angle_deg: target steering angle in degrees
    speed_deg_s: steering speed in deg/s
    counter: message counter
    enable: whether steering control is enabled
  """
  values = {
    'angle': angle_deg if enable else 0.0,
    'speed': speed_deg_s,
    'counter': counter,
    'mode': 1 if enable else 0,
    'reserved': 0,
    'time': 0,
  }
  return packer.make_can_msg('frontSteeringReq', 0, values)


def create_rear_steering_command(packer, angle_deg, speed_deg_s, counter, enable):
  """Create rearSteeringReq (0x164).

  Args:
    packer: CANPacker instance
    angle_deg: target steering angle in degrees (same as front)
    speed_deg_s: steering speed in deg/s
    counter: message counter
    enable: whether steering control is enabled
  """
  values = {
    'angle': angle_deg if enable else 0.0,
    'speed': speed_deg_s,
    'counter': counter,
    'mode': 1 if enable else 0,
    'reserved': 0,
    'time': 0,
  }
  return packer.make_can_msg('rearSteeringReq', 0, values)


def create_brake_command(packer, pressure_bar, counter, enable):
  """Create EHBReqMsg (0x150).

  Args:
    packer: CANPacker instance
    pressure_bar: brake pressure in bar
    counter: 4-bit message counter (0-15)
    enable: whether brake control is enabled
  """
  values = {
    'enable': 1 if enable else 0,
    'parkActive': 0,
    'slope': 0,
    'pressure': int(round(pressure_bar)) if enable else 0,
    'counter': counter & 0xF,
    'checksum': 0,  # will be calculated below
  }

  # Calculate checksum: sum of all bytes except checksum byte, mod 256
  dat = packer.make_can_msg('EHBReqMsg', 0, values)
  # dat is (address, data_bytes, bus) - data_bytes is the raw bytes
  raw = dat[1]
  checksum = sum(raw[:7]) & 0xFF  # sum first 7 bytes, checksum is byte 7
  values['checksum'] = checksum
  return packer.make_can_msg('EHBReqMsg', 0, values)


def create_epb_command(packer, engage):
  """Create EPBReqMsg (0x2B0).

  Args:
    packer: CANPacker instance
    engage: True to engage parking brake, False to release
  """
  values = {
    'request': 1 if engage else 0,
  }
  return packer.make_can_msg('EPBReqMsg', 0, values)
