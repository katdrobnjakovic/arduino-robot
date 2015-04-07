import sys
sys.path.append("/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages")
from flask import Flask, render_template, url_for
from robot_communication import RobotController

app = Flask(__name__)


@app.route('/')
def index():
    robotComm = RobotController()
    return render_template('index.html')

@app.route('/forward')
def forward():
    print 'moving forwarddddd'
    return 'Moving forward!' 

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

if __name__ == '__main__':
    app.debug = True
    app.run()