import constants
import time
import datetime
import threading

log_lock = threading.Lock()
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
    with log_lock:
      single_log = remote_backlog.pop(0)
    return single_log
  except IndexError:
    return None

def _local_log(message):
  print(message)

def _remote_log(message):
  with log_lock:
    remote_backlog.append(message)

def _append_timestamp(message):
  st = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
  return st + ": " + message 