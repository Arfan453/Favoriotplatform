from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO
from datetime import datetime

# Create the Flask application instance.
app = Flask(__name__)

# Setting a secret key for the Flask app. This is used to sign session cookies for security purposes.
app.config['SECRET_KEY'] = 'FIQXQDIN'

# Create a SocketIO instance by passing the Flask app to it.
socketio = SocketIO(app)

# Defining a route for the root URL ('/') of the application.
# This route handles both GET (to show the form) and POST (to handle the form submission).
@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        # Extract form data from the POST request
        apikey = request.form['apikey']  # Get the API key entered by the user
        device_id = request.form['device_id']  # Get the device developer ID entered by the user
        datetime = request.form['datetime']  # Get the selected date and time
        
        desired_date = convert_datetime(datetime)

        # Store the form data in a dictionary to pass to the template
        form_data = {
            'apikey': apikey,
            'device_id': device_id,
            'datetime': desired_date
        }

        # You can now use the data however you want, such as printing or passing it to a function.
        print(f"API Key: {apikey}")
        print(f"Device ID: {device_id}")
        print(f"Selected Date and Time: {desired_date}")
        
        # Pass form_data to the 'submit.html' template to render the response
        return render_template('submit.html', form_data=form_data)

    # If the request is a GET request, render the form.
    return render_template('home.html')

@app.route("/submit", methods=["GET", "POST"])
def submit():
    # Retrieve the form data
    apikey = request.form.get("apikey")
    device_id = request.form.get("device_id")
    datetime = request.form.get("datetime")  # Get the datetime value

    # Create a dictionary with the form data
    form_data = {
        "apikey": apikey,
        "device_id": device_id,
        "datetime": datetime
    }

    return jsonify(form_data)

def convert_datetime(input_date):
    # Parse the input date string (format: 2024-12-17T20:51)
    input_dt = datetime.strptime(input_date, "%Y-%m-%dT%H:%M")
    
    # Format the datetime to the desired format (2016-09-03T01:39:39.473Z)
    output_date = input_dt.strftime("%Y-%m-%dT%H:%M:%S.") + f"{input_dt.microsecond // 1000:03d}Z"
    
    return output_date

# The entry point for running the Flask app. This condition ensures the app is only run
# if this script is executed directly (not when imported as a module).
if __name__ == '__main__':
    # Running the Flask app with WebSocket support using SocketIO. 
    socketio.run(app, debug=True, allow_unsafe_werkzeug=True)
