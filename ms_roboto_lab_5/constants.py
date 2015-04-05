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
  'ip': '127.0.0.1',
  'port': 32114,
  # Length of time (in seconds) the UDP communication will wait
  'timeout': None,
  'buffer_size': 1024
}

LOGGING = {
  'local_log': True,
  'remote_log': False,
  'timestamp_enabled': True
}