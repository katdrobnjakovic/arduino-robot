import constants

def log(message):
  """
  This is here to allow simple logging on both the local server and the
  website with timestamps (required by the lab).
  """
  if constants.LOGGING['timestamps_enabled']:
    message = _append_timestamp(message)

  if constants.LOGGING['local_log']:
    _local_log(message)

  if constants.LOGGING['remote_log']:
    _remote_log(message)

def _local_log(message):
  pass

def _remote_log(message):
  pass

def _append_timestamp(message):
  pass