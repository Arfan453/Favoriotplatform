from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

@app.route("/", methods=["GET"])
def home():
    return render_template("home.html")
    
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

if __name__ == '__main__':
    app.run(debug=True)
