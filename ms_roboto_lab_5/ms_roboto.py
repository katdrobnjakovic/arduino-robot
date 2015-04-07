import sys
sys.path.append("/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages")
from flask import Flask, render_template, url_for

app = Flask(__name__)


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/forward')
def forward():
    print url_for('forward')

@app.route('/backward')
def backward():
    print url_for('backward')

@app.route('/left')
def left():
    print url_for('left')

@app.route('/right')
def right():
    print url_for('right')

@app.route('/distance')
def distance():
    print url_for('distance')

@app.route('/temperature')
def temperature():
    print url_for('temperature')

if __name__ == '__main__':
    app.debug = True
    app.run()