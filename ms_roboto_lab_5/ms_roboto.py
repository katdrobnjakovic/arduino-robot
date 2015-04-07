import sys
sys.path.append("/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages")
from flask import Flask, render_template, url_for
from robot_communication import RobotController
from log import retreive_single_log

app = Flask(__name__)

robotComm = RobotController()
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/forward')
def forward():
    print 'moving forwarddddd'
    return robotComm.move_forward(10) 

@app.route('/backward')
def backward():
    print 'moving backwardddddd'
    return 'Moving forward!'

@app.route('/left')
def left():
    print 'turning leftttttt'
    return 'Turning Left!'

@app.route('/right')
def right():
    print 'turning righttttt'
    return 'Turning right!'

@app.route('/distance')
def distance():
    print 'Getting distance'
    return 'Getting distance!'

@app.route('/temperature')
def temperature():
    print 'Getting temperature'
    return 'Getting temperature!'

@app.route('/printlog')
def printlog():
    #retreive_remote_log()
    plog = retreive_single_log()  
    if plog is None:
        return ("", 204)
    else: 
        print 'Here is the logggggg'
        return retreive_single_log()  

if __name__ == '__main__':
    app.debug = True
    app.run()