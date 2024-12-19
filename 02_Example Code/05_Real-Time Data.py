# Import necessary modules
from flask import Flask, render_template, request  # Flask is the web framework, render_template for rendering HTML files
from flask_socketio import SocketIO  # Flask-SocketIO enables WebSocket communication
from random import random  # For generating random sensor values
from threading import Lock  # To ensure thread-safe operations
from datetime import datetime  # To get the current date and time

"""
Global Variables and Initialization
"""
thread = None  # Background thread variable to hold the thread instance
thread_lock = Lock()  # Thread lock to ensure thread-safe access

# Create a Flask application instance
app = Flask(__name__)  
app.config['SECRET_KEY'] = 'donsky!'  # Secret key for Flask app for session management/security
socketio = SocketIO(app, cors_allowed_origins='*')  # Initialize SocketIO with the app, allowing cross-origin requests

"""
Function to get the current date and time in a readable format
"""
def get_current_datetime():
    now = datetime.now()  # Fetch the current datetime
    return now.strftime("%m/%d/%Y %H:%M:%S")  # Format the datetime as MM/DD/YYYY HH:MM:SS

"""
Background thread function to simulate sensor data generation
"""
def background_thread():
    print("Generating random sensor values")  # Log that the thread is running
    while True:
        # Generate a random dummy sensor value between 0 and 100, rounded to 3 decimal places
        dummy_sensor_value = round(random() * 100, 3)
        # Emit the sensor value and current timestamp to all connected clients via WebSocket
        socketio.emit('updateSensorData', {'value': dummy_sensor_value, "date": get_current_datetime()})
        socketio.sleep(1)  # Pause for 1 second before generating the next value

"""
Route to serve the root page (index)
"""
@app.route('/')
def index():
    # Render the 'yt.html' template as the homepage
    return render_template('yt.html')

"""
WebSocket event handler for client connection
"""
@socketio.on('connect')  # This decorator defines a handler for the 'connect' event
def connect():
    global thread  # Access the global thread variable
    print('Client connected')  # Log the connection event
    
    # Start the background thread if it's not already running
    with thread_lock:  # Ensure thread-safe access using the lock
        if thread is None:
            # Start the background thread for generating sensor data
            thread = socketio.start_background_task(background_thread)

"""
WebSocket event handler for client disconnection
"""
@socketio.on('disconnect')  # This decorator defines a handler for the 'disconnect' event
def disconnect():
    # Log the disconnection event along with the client's session ID
    print('Client disconnected', request.sid)

"""
Main entry point of the application
"""
if __name__ == '__main__':
    # Run the Flask application with SocketIO
    socketio.run(app, allow_unsafe_werkzeug=True)
