import constants
import time
import datetime

remote_backlog = []

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

def retreive_single_log():
  try:
   return remote_backlog.pop(0)
  except IndexError:
    return None

def retreive_remote_log():
  temp_log = [msg for msg in remote_backlog]
  remote_backlog = []
  return temp_log

def _local_log(message):
  print(message)

def _remote_log(message):
  remote_backlog.append(message)

def _append_timestamp(message):
  st = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
  return st + ": " + message 