"""
Names: Craig Bryan
       Kat Drobnjakovic

Course Code: SEG 4145
Lab Number: 5
File name: log.py
Date: April 9, 2015

Description:
Methods used to create log messages to be displayed with their timestamp 
""" 

import constants
import time
import datetime
import threading

#lock for the log thread
log_lock = threading.RLock()
remote_backlog = []

"""
Name
log

Description
This is here to allow simple logging on both the local server and the
website with timestamps.

Parameters
message - the log

Returns
Void 
"""
def log(message):
  if constants.LOGGING['timestamps_enabled']:
    message = _append_timestamp(message)

  if constants.LOGGING['local_log']:
    _local_log(message)

  if constants.LOGGING['remote_log']:
    _remote_log("DUMBY LOG")
    _remote_log(message)

"""
Name
log

Description
Gets the first log.

Parameters
None

Returns
The first log. 
"""
def retreive_single_log():
  with log_lock:
    if remote_backlog:
      print("BEFORE SINGLE LOG")
      single_log = remote_backlog.pop(0)
      print("THIS IS THE SINGLE LOG" + single_log)
      return single_log
    else:
      print("EMPTY LOG")
      return None
    

"""
Name
log

Description
Prints the log to the local server.

Parameters
message - the log

Returns
Void
"""
def _local_log(message):
  print(message)

"""
Name
log

Description
Adds a new log to the remote_backlog

Parameters
message - the log

Returns
Void
"""
def _remote_log(message):
  with log_lock:
    print("Adding message to log")
    remote_backlog.append(message)

"""
Name
log

Description
Appends the timestamp to the log

Parameters
message - the log

Returns
Returns the log with its timestamp  
"""
def _append_timestamp(message):
  st = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
  return st + ": " + message 