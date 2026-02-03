# 12. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong 

# 3 Implementierung der REST-API mit Flask

from flask_cors import CORS # Cors Header
from flask import Flask, jsonify, request
import json

app = Flask(__name__)

CORS(app, resources={r"/weather*": {"origins": ["http://localhost:5173", "http://127.0.0.1:5173"]}})

# Laden der Wetterdaten

# Funktion zum Laden der Wetterdaten
def load_data():
    with open("weather.json", "r", encoding="utf-8") as f:
        daten = json.load(f)
    return daten

# Funktion zum Speichern der Wetterdaten
def save_data(data):
    with open("weather.json", "w", encoding="utf-8") as f:
        json.dump(data, f, indent=4, ensure_ascii=False)


# Beispiele für Umsetzen HTTP GET-Methode

# Gibt alle in weather.json gespeicherten Messungen zurück

@app.route("/weather", methods=["GET"])
def get_all_weather():
    """Gibt alle Wetterdatensätze zurück."""
    return jsonify(load_data()), 200

# Gibt nur die Messdaten der Wetterstation „Botanischer-Garten-1“ zurück
   
@app.route("/weather/station/<station_id>", methods=["GET"])
def get_weather_by_station(station_id):
    """Filtert Messwerte nach station-id.
       Gibt passenden Datensatz zur station-id wieder."""

    data = load_data()
    result = [d for d in data if d.get("station-id", "").lower() == station_id.lower()]
    if result:
        return jsonify(result), 200
    return jsonify({"error": f"Station '{station_id}' nicht gefunden."}), 404

# Gibt alle Messungen des Kanals aus

@app.route("/weather/channel/<channel_id>", methods=["GET"])
def get_channel_measurement(channel_id):

    data = load_data()
    result = [d for d in data if d.get("channel", "") == channel_id]
    # nur Messungen wiedergeben
    result = [
        {
            "channel": d["channel"],
            "longname": d["longname"],
            "value": d["value"],
            "unit": d["unit"]
        }
        for d in result
    ]

    if result:
        return jsonify(result), 200
    return jsonify({"error": f"Channel '{channel_id}' nicht gefunden."}), 404

# Gibt den einzelnen Datensatz mit der angegebenen ID zurück

@app.route("/weather/id/<s_ID>", methods=["GET"])
def get_id_data(s_ID):

    data = load_data()
    result = [d for d in data if d.get("id", "").lower() == s_ID.lower()]

    if result:
        return jsonify(result), 200
    return jsonify({"error": f"ID '{s_ID}' nicht gefunden."}), 404


# 3.2 CRUD-Funktionalität

# POST
# Fügt einen neuen Wetterdatensatz hinzu

@app.route("/weather", methods=["POST"])
def add_weather():
    new_data = request.get_json()

    if not new_data:
        return jsonify({"error": "Kein JSON-Body übergeben"}), 400

    data = load_data()

    if any(d["id"] == new_data.get("id") for d in data):
        return jsonify({"error": "ID existiert bereits"}), 400

    data.append(new_data)
    save_data(data)

    return jsonify({"message": "Datensatz hinzugefügt", "data": new_data}), 201

# PUT
# Aktualisiert Wetterdaten für eine Station / Stadt

@app.route("/weather/<stadt>", methods=["PUT"])
def update_weather(stadt):
    update_data = request.get_json()

    if not update_data:
        return jsonify({"error": "Kein JSON-Body übergeben"}), 400

    data = load_data()
    updated = False

    for d in data:
        # Wir haben nun Stadt auf Station-ID gemappt, weil keine Stadt in der weather.json vorhanden ist 
        if d.get("station-id", "").lower() == stadt.lower():
            d.update(update_data)
            updated = True

    if not updated:
        return jsonify({"error": f"Station '{stadt}' nicht gefunden"}), 404

    save_data(data)
    return jsonify({"message": f"Daten für Station '{stadt}' aktualisiert"}), 200

# DELETE
# Löscht alle Wetterdaten einer Station / Stadt

@app.route("/weather/<stadt>", methods=["DELETE"])
def delete_weather(stadt):
    data = load_data()
    new_data = [d for d in data if d.get("station-id", "").lower() != stadt.lower()]

    if len(new_data) == len(data):
        return jsonify({"error": f"Station '{stadt}' nicht gefunden"}), 404

    save_data(new_data)
    return jsonify({"message": f"Station '{stadt}' gelöscht"}), 200

# 12. UZ
@app.route("/weather/id/<data_id>", methods=["PUT"])
def update_single_weather(data_id):
    update_data = request.get_json()
    if not update_data:
        return jsonify({"error": "Kein JSON-Body übergeben"}), 400

    data = load_data()
    updated = False

    for entry in data:
        if entry.get("id", "").lower() == data_id.lower():
            entry.update(update_data)
            updated = True
            break

    if not updated:
        return jsonify({"error": f"Datensatz mit ID '{data_id}' nicht gefunden"}), 404

    save_data(data)
    return jsonify({"message": f"Datensatz '{data_id}' aktualisiert"}), 200

# Main entry point
if __name__ == "__main__":
    app.run(debug=True)
