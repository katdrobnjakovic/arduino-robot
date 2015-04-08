import sys, multiprocessing, threading
import atexit
sys.path.append("/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages")
from flask import Flask, render_template, url_for
from robot_communication import RobotController
from log import retreive_single_log


app = None

POOL_TIME = 5 #Seconds

# variables that are accessible from anywhere
commonDataStruct = []
# lock to control access to variable
dataLock = threading.Lock()
# thread handler
yourThread = threading.Thread()

def create_app():
    global app 
    app = Flask(__name__)

    def interrupt():
        global yourThread
        yourThread.cancel()

    def doStuff():
        global commonDataStruct
        global yourThread
        robotComm = RobotController()
        with dataLock:
            # Do your stuff with commonDataStruct Here
            while True:
                if commonDataStruct:
                    command = commonDataStruct.pop()
                    if command[1] is None:
                        getattr(robotComm, command[0])()
                    else:
                        getattr(robotComm, command[0])(command[1])

    def doStuffStart():
        # Do initialisation stuff here
        global yourThread
        # Create your thread
        yourThread = threading.Timer(POOL_TIME, doStuff, ())
        yourThread.start()

    # Initiate
    doStuffStart()
    # When you kill Flask (SIGTERM), clear the trigger for the next thread
    atexit.register(interrupt)
    


    @app.route('/')
    def index():
        return render_template('index.html')


    @app.route('/forward')
    def forward():
        print 'moving forwarddddd'
        commonDataStruct.append(('move_forward', 10)) 
        return ''

    @app.route('/backward')
    def backward():
        print 'moving backwardddddd'
        commonDataStruct.append(('move_backwards', 10)) 
        return ''

    @app.route('/left')
    def left():
        print 'turning leftttttt'
        commonDataStruct.append(('turn_right', 45)) 
        return ''

    @app.route('/right')
    def right():
        print 'turning righttttt'
        commonDataStruct.append(('turn_left', 45)) 
        return ''

    @app.route('/distance')
    def distance():
        print 'Getting distance'
        commonDataStruct.append(('read_distance', None)) 
        return ''

    @app.route('/temperature')
    def temperature():
        print 'Getting temperature'
        commonDataStruct.append(('read_temperature', None)) 
        return ''

    @app.route('/printlog')
    def print_log():
        #retreive_remote_log()
        plog = retreive_single_log()  
        if plog is None:
            return ("", 204)
        else: 
            print 'Here is the logggggg'
            return retreive_single_log()  
    return app

if __name__ == '__main__':
    print('IN MAIN')
    app = create_app() 
    #app.debug = True
    app.run()