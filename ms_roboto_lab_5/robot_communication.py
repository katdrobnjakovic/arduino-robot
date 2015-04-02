import socket
import constants

class RobotController:
  """
  Translates the commands into the command strings the robot can understand
  and sends them to the robot using a UDPCommunicator
  """ 
  def __init__(self):
    self._communicator = UDPCommunicator(constants.ROBOT_COMM['ip'],
                                         constants.ROBOT_COMM['port'])
    self._communicator.connect()

  def move_forward(self, amount):
    pass

  def move_backwards(self, amount):
    pass

  def turn_right(self, amount):
    pass

  def turn_left(self, amount):
    pass

class UDPCommunicator:
  """
  Deals with the communication to and from the robot using UDP packets
  """
  def __init__(self, initial_ip, initial_port):
    self._ip = initial_ip
    self._port = initial_port
    self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self._connected = False

  def connect(self):
    if self._connected:
      return

    self._socket.bind(self._ip, self._port)
    data, addr = self._receive_from_peer()
    self._ip, self._port = addr
    self._set_timeout()
    self._connected = True

  def send(self, message):
    self._ensure_connected()
    self._send_to_peer(message)

  def receive(self):
    self._ensure_connected()
    data, addr = self._receive_from_peer()

  def _ensure_connected(self):
    if not self._connected:
      raise ValueError("Communicator is not yet connected")

  def _send_to_peer(self, message):
    self._socket.sendto(message, (self._ip, self._port))

  def _receive_from_peer(self):
    return self._socket.recvfrom(constants.ROBOT_COMM['buffer_size'])

  def _set_timeout(self):
    self._socket.settimeout(constants.ROBOT_COMM['timeout'])