"""
Names: Craig Bryan
       Kat Drobnjakovic

Course Code: SEG 4145
Lab Number: 5
File name: ms_roboto.py
Date: April 9, 2015

Description:
The routes communicating the robot commands. 
Multi-threading implemented with multiprocessing to maintain communication with the robot 
render the web application simultaneously. 
""" 
import sys
import multiprocessing
import threading
import atexit
sys.path.append("/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages")
from flask import Flask, render_template, url_for
from robot_communication import RobotController
from log import retreive_single_log


app = None

#execute the thread every 5 seconds
POOL_TIME = 5 

#list variable that is accessible from anywhere - contains the robot commands
robo_commands_list = []
#lock to control access to robot list of commands
data_lock = threading.Lock()
#thread handler
robot_thread = threading.Thread()

"""
Name
create_app

Description
Creates the Flask app - makes the routes for robot communication and multiprocesses 
to continue the robot communication and render the web page simultaneously

Parameters
None

Returns
The created app. 
"""

def create_app():
    global app 
    app = Flask(__name__)

    """
    Name
    interrupt
    
    Description
    Interrupts the thread
    
    Parameters
    None
    
    Returns
    Void 
    """
    def interrupt():
        global robot_thread
        robot_thread.cancel()

    """
    Name
    send_commands
    
    Description
    Uses a thread to send commands from the robot_commands_list to the robot via the RobotController

    Parameters
    None
    
    Returns
    Void 
    """
    def send_commands():
        global robo_commands_list
        global robot_thread
        robot_comm = RobotController()
        with data_lock:
            # send the commands from the robo_commands_list to the robot
            while True:
                if robo_commands_list:
                    command = robo_commands_list.pop()
                    if command[1] is None:
                        getattr(robot_comm, command[0])()
                    else:
                        getattr(robot_comm, command[0])(command[1])

    """
    Name
    start_send_commands
    
    Description
    Initialize and create the robot thread 
    
    Parameters
    None
    
    Returns
    Void 
    """                    
    def start_send_commands():
        # Initialize the robot thread
        global robot_thread
        # Create the robot thread
        robot_thread = threading.Timer(POOL_TIME, send_commands, ())
        robot_thread.start()


    # Initiate
    start_send_commands()
    # When you kill Flask (SIGTERM), clear the trigger for the next thread
    atexit.register(interrupt)
    

    """
    Name
    index
    
    Description
    Renders the index.html template view for the root route

    Parameters
    None
    
    Returns
    The template to render the view 
    """  
    @app.route('/')
    def index():
        return render_template('index.html')

    """
    Name
    forward
    
    Description
    Sends the move_forward command to the robo_commands_list to be 
    communicated to the robot. The robot will move forward 10. 

    Parameters
    None
    
    Returns
    Empty string 
    """
    @app.route('/forward')
    def forward():
        robo_commands_list.append(('move_forward', 10)) 
        return ''

    """
    Name
    backward
    
    Description
    Sends the move_backwards command to the robo_commands_list to be 
    communicated to the robot. The robot will move backwards 10. 

    Parameters
    None
    
    Returns
    Empty string 
    """
    @app.route('/backward')
    def backward():
        robo_commands_list.append(('move_backwards', 10)) 
        return ''

    """
    Name
    left
    
    Description
    Sends the turn_left command to the robo_commands_list to be 
    communicated to the robot. The robot will make a 45 degree
    turn to the left. 

    Parameters
    None
    
    Returns
    Empty string 
    """
    @app.route('/left')
    def left():
        robo_commands_list.append(('turn_left', 45)) 
        return ''

    """
    Name
    right
    
    Description
    Sends the turn_right command to the robo_commands_list to be 
    communicated to the robot. The robot will make a 45 degree
    turn to the right. 

    Parameters
    None
    
    Returns
    Empty string 
    """
    @app.route('/right')
    def right():
        robo_commands_list.append(('turn_right', 45)) 
        return ''

    """
    Name
    distance
    
    Description
    Sends the read_distance command to the robo_commands_list to be 
    communicated to the robot. 

    Parameters
    None
    
    Returns
    Empty string 
    """
    @app.route('/distance')
    def distance():
        robo_commands_list.append(('read_distance', None)) 
        return ''

    """
    Name
    temperature
    
    Description
    Sends the read_temperature command to the robo_commands_list to be 
    communicated to the robot. 

    Parameters
    None
    
    Returns
    Empty string 
    """
    @app.route('/temperature')
    def temperature():
        robo_commands_list.append(('read_temperature', None)) 
        return ''

    """
    Name
    printlog
    
    Description
    Retrieves one log to display in the view.  

    Parameters
    None
    
    Returns
    The single log output to display in the view.  
    """
    @app.route('/printlog')
    def print_log():
        p_log = retreive_single_log()  
        if p_log is None:
            return ("", 204)
        else: 
            return retreive_single_log()  
    return app

if __name__ == '__main__':
    #create the Flask app
    app = create_app() 
    #app.debug = True
    app.run()