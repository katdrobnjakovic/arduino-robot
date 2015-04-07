import socket
import constants
import log

class RobotController:
  """
  Translates the commands into the command strings the robot can understand
  and sends them to the robot using a UDPCommunicator
  """ 
  def __init__(self):
    self._communicator = UDPCommunicator(constants.ROBOT_COMM['ip'],
                                         constants.ROBOT_COMM['port'])
    while not self._communicator.connected:
      try:
        log.log("Connecting to peer")
        connection_response = self._communicator.connect()
      except socket.timeout:
        log.log("Timed out waiting to connect to the robot. Retrying")

    if connection_response is not None:
      log.log("Successfully connected. Robot says: " + connection_response)
    else:
      log.log("Error connecting to robot")

  def move_forward(self, amount):
    log.log("Telling robot to move forward")
    command = constants.CMD_CHARS['forward'] + " " + str(amount)
    self._send_command(command)
    self._receive_response()

  def move_backwards(self, amount):
    log.log("Telling robot to move backwards")
    command = constants.CMD_CHARS['backwards'] + " " + str(amount)
    self._send_command(command)
    self._receive_response()

  def turn_right(self, amount):
    log.log("Telling robot to turn right")
    command = constants.CMD_CHARS['right'] + " " + str(amount)
    self._send_command(command)
    self._receive_response()

  def turn_left(self, amount):
    log.log("Telling robot to turn left")
    command = constants.CMD_CHARS['left'] + " " + str(amount)
    self._send_command(command)
    self._receive_response()

  def read_distance(self, amount):
    log.log("Telling robot to measure distance")
    command = constants.CMD_CHARS['distance']
    self._send_command(command)
    self._receive_response()

  def read_temperature(self, amount):
    log.log("Telling robot to measure temperature")
    command = constants.CMD_CHARS['temperature']
    self._send_command(command)
    self._receive_response()

  def _send_command(self, cmd):
    log.log("Sending to robot: " + cmd)
    self._communicator.send(cmd)
    log.log("Successfully sent message to robot")

  def _receive_response(self):
    log.log("Receiving response")
    response = self._communicator.receive()
    response_flag = response.split(" ")[0]
    
    if response_flag == constants.CMD_CHARS['success']:
      log.log("Successfully executed movement command")
      return (True, None)
    elif response_flag == constants.CMD_CHARS['result']:
      log.log("Successfully executed data retrieval command. Result: " 
        + response.split(" ")[1])
      return (True, response.split(" ")[1])
    elif response_flag == constants.CMD_CHARS['error']:
      log.log("Error executing command: " + response.split(" ")[1])
      return (False, response.split(" ")[1])
    else:
      log.log("Unknown response: " + response)
      return (False, None)

class UDPCommunicator:
  """
  Deals with the communication to and from the robot using UDP packets
  """
  def __init__(self, initial_ip, initial_port):
    self._ip = initial_ip
    self._port = initial_port
    self.connected = False

  def connect(self):
    if self.connected:
      return None

    self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self._socket.bind((self._ip, self._port))
    self._set_timeout()
    try:
      data, addr = self._receive_from_peer()
    except socket.timeout as e:
      self._socket.close()
      raise e

    self._ip, self._port = addr
    self.connected = True
    return data

  def send(self, message):
    self._ensure_connected()
    self._send_to_peer(message)

  def receive(self):
    msg = self._receive_from_peer()
    return msg[0] #msg is a tuple of (data, addr)

  def _ensure_connected(self):
    if not self.connected:
      raise ValueError("Communicator is not yet connected")

  def _send_to_peer(self, message):
    self._socket.sendto(message, (self._ip, self._port))

  def _receive_from_peer(self):
    return self._socket.recvfrom(constants.ROBOT_COMM['buffer_size'])

  def _set_timeout(self, timeout = -1):
    if timeout != -1:
      self._socket.settimeout(timeout)
    else:
      self._socket.settimeout(constants.ROBOT_COMM['timeout'])