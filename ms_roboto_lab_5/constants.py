CMD_CHARS = {
  'forward': 'f',
  'backwards': 'b',
  'left': 'l',
  'right': 'r',
  'distance': 'p',
  'temperature': 't',
  'success': 'd',
  'result': 'w',
  'error': 'e'
}

ROBOT_COMM = {
  'ip': '0.0.0.0',
  'port': 32114,
  # Length of time (in seconds) the UDP communication will wait
  'timeout': 5,
  'buffer_size': 1024
}

LOGGING = {
  'local_log': True,
  'remote_log': True,
  'timestamps_enabled': True
}