"""
Names: Craig Bryan
       Kat Drobnjakovic

Course Code: SEG 4145
Lab Number: 5
File name: ms_roboto
Date: April 9, 2015

Description:
The robot controller translates the commands into the command strings the robot can understand
  and sends them to the robot using a UDPCommunicator 
""" 
import socket
import constants
import log

class RobotController:
  
  def __init__(self):
    self._communicator = UDPCommunicator(constants.ROBOT_COMM['ip'],
                                         constants.ROBOT_COMM['port'])
    self.last = 'c'

    while not self._communicator.connected:
      try:
        log.log("Connecting to peer")
        #added to fake log 
        connection_response = self._communicator.connect() #og.log("Error connecting to robot")
        #self._communicator.connected = True  
        #log.log("Telling robot to move forward")   
      except socket.timeout:
        log.log("Timed out waiting to connect to the robot. Retrying")

    if connection_response is not None:
      log.log("Successfully connected. Robot says: " + connection_response)
    else:
      log.log("Error connecting to robot")
      
  """
  Name
  move_forward

  Description
  Sends a move forward command to the robot via the UDP communication protocol

  Parameters
  Amount - the number of centimeters to move forward

  Returns
  A tuple in the form (Boolean, String/None). The boolean is true if the command
  was successful, false otherwise. The String contains the message from the
  robot, or an error message
  """
  def move_forward(self, amount):
    log.log("Telling robot to move forward")
    command = constants.CMD_CHARS['forward'] + " " + str(amount)
    self._send_once(command)

  """
  Name
  move_backwards

  Description
  Sends a move backwards command to the robot via the UDP communication protocol

  Parameters
  Amount - the number of centimeters to move backwards

  Returns
  A tuple in the form (Boolean, String/None). The boolean is true if the command
  was successful, false otherwise. The String contains the message from the
  robot, or an error message
  """
  def move_backwards(self, amount):
    log.log("Telling robot to move backwards")
    command = constants.CMD_CHARS['backwards'] + " " + str(amount)
    self._send_once(command)

  """
  Name
  turn_right

  Description
  Sends a turn right command to the robot via the UDP communication protocol

  Parameters
  Amount - the number of degrees to turn clockwise

  Returns
  A tuple in the form (Boolean, String/None). The boolean is true if the command
  was successful, false otherwise. The String contains the message from the
  robot, or an error message
  """
  def turn_right(self, amount):
    log.log("Telling robot to turn right")
    command = constants.CMD_CHARS['right'] + " " + str(amount)
    self._send_once(command)

  """
  Name
  turn_left

  Description
  Sends a turn left command to the robot via the UDP communication protocol

  Parameters
  Amount - the number of degrees to turn counter clockwise

  Returns
  A tuple in the form (Boolean, String/None). The boolean is true if the command
  was successful, false otherwise. The String contains the message from the
  robot, or an error message
  """
  def turn_left(self, amount):
    log.log("Telling robot to turn left")
    command = constants.CMD_CHARS['left'] + " " + str(amount)
    self._send_once(command)

  """
  Name
  read_distance

  Description
  Sends a read distance to the robot via the UDP communication protocol

  Parameters
  None

  Returns
  A tuple in the form (Boolean, String/None). The boolean is true if the command
  was successful, false otherwise. The String contains the message from the
  robot, or an error message
  """
  def read_distance(self):
    log.log("Telling robot to measure distance")
    command = constants.CMD_CHARS['distance'] + ""
    self.last = 'd'
    self._send_until_successful(command)

  """
  Name
  read_temperature

  Description
  Sends a read temperature to the robot via the UDP communication protocol

  Parameters
  None

  Returns
  A tuple in the form (Boolean, String/None). The boolean is true if the command
  was successful, false otherwise. The String contains the message from the
  robot, or an error message
  """
  def read_temperature(self):
    log.log("Telling robot to measure temperature")
    command = constants.CMD_CHARS['temperature']
    self.test = 't'
    self._send_until_successful(command)

  def _send_until_successful(self, command):
    self._communicator.set_timeout(1);
    successful = False
    
    while not successful:
      try:
        self._send_command(command)
        self._receive_response()
        successful = True
      except socket.timeout:
        log.log("Timed out. Retrying")

  def _send_once(self, command):
    self._communicator.set_timeout(constants.ROBOT_COMM['timeout'])

    try:
      self._send_command(command)
      self._receive_response()
    except socket.timeout:
      log.log("Timeout sending command or receiving response. " 
        + "Please try again.")

  def _send_command(self, command):
    log.log("Sending to robot: " + command)
    self._communicator.send(command)
    log.log("Successfully sent message to robot")

  def _receive_response(self):
    log.log("Receiving response")
    response = self._communicator.receive()
    if response is None:
      log.log("No response received. Packet must've been lost")
      return (False, None)

    response_flag = response.split(" ")[0]
    
    if response_flag == constants.CMD_CHARS['success']:
      log.log("Successfully executed movement command")
      return (True, None)
    elif response_flag == constants.CMD_CHARS['result']:
      if self.last == 't':
        log.log("Ambient temperature: " + response[1:] + " degree C")
      elif self.last == 'd':
        log.log("Distance to nearest obstacle: " + response[1:] + "cm")
      else:
        log.log("Successfully executed data retrieval command. Result: " 
          + response[1:])
      return (True, response[1:])
    elif response_flag == constants.CMD_CHARS['error']:
      log.log("Error executing command: " + response[1:])
      return (False, response[1:])
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

  """
  Name
  connect

  Description
  Establishes communication with the robot. It waits and retries until the
  robot sends a message.

  Parameters
  None

  Returns
  None
  """
  def connect(self):
    if self.connected:
      return None

    self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self._socket.bind((self._ip, self._port))
    self.set_timeout()
    try:
      data, addr = self._receive_from_peer()
    except socket.timeout as e:
      self._socket.close()
      raise e

    self._ip, self._port = addr
    self.connected = True
    return data

  """
  Name
  send

  Description
  Sends a message to ther robot over UDP 

  Parameters
  Message - the message to send to the robot

  Returns
  None
  """
  def send(self, message):
    self._ensure_connected()
    self._send_to_peer(message)

  """
  Name
  receive

  Description
  Waits for a message from the robot, sent over UDP. Will timeout if waiting 
  too long. 

  Parameters
  Message - the message to send to the robot

  Returns
  None
  """
  def receive(self):
    msg = self._receive_from_peer()
    return msg[0] #msg is a tuple of (data, addr)

  def set_timeout(self, timeout = -1):
    if timeout != -1:
      self._socket.settimeout(timeout)
    else:
      self._socket.settimeout(constants.ROBOT_COMM['timeout'])

  def _ensure_connected(self):
    if not self.connected:
      raise ValueError("Communicator is not yet connected")

  def _send_to_peer(self, message):
    self._socket.sendto(message, (self._ip, self._port))

  def _receive_from_peer(self):
    return self._socket.recvfrom(constants.ROBOT_COMM['buffer_size'])