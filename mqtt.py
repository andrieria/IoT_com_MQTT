import threading
import paho.mqtt.client as mqtt
from flask import Flask, jsonify, render_template_string
from geopy.geocoders import Nominatim
import folium

# Configurações do MQTT
#mqtt_broker = "test.mosquitto.org"
mqtt_broker = "broker.hivemq.com"
mqtt_port = 1883
mqtt_topic = "veiculo/localizacao"

# Cria um cliente MQTT
client = mqtt.Client()

app = Flask(__name__)

# Variável para armazenar a última mensagem recebida
last_location = None

# Função callback que é chamada quando a conexão com o broker é estabelecida
def on_connect(client, userdata, flags, rc):
    print(f"Conectado ao broker MQTT com código {rc}")
    client.subscribe(mqtt_topic)

# Função callback que é chamada quando uma mensagem é recebida
def on_message(client, userdata, msg):
    global last_location
    last_location = msg.payload.decode()
    print(f"Tópico: {msg.topic}\nMensagem: {last_location}")

client.on_connect = on_connect
client.on_message = on_message

def run_mqtt():
    client.connect(mqtt_broker, mqtt_port, 60)
    client.loop_forever()

@app.route('/')
def index():
    if last_location:
        # Divide a string da mensagem recebida e remove a unidade 'meters'
        lat_str, lon_str, alt_str = last_location.split('|')
        lat = float(lat_str.split(":")[1].strip())
        lon = float(lon_str.split(":")[1].strip())
        alt = float(alt_str.split(":")[1].strip().split()[0])  # Remove 'meters'

        geolocator = Nominatim(user_agent="geoapiExercises")
        location = geolocator.reverse(f"{lat}, {lon}", language='en')
        address = location.raw.get('address', {})
        street = address.get('road', '')
        neighborhood = address.get('neighbourhood', '')
        city = address.get('city', '')
        state = address.get('state', '')
        region = address.get('region', '')
        country = address.get('country', '')

        # Cria um mapa com Folium
        mapa = folium.Map(location=[lat, lon], zoom_start=15)
        folium.Marker([lat, lon], popup=f"Location: {street}, {neighborhood}, {city}, {state}, {region}, {country}").add_to(mapa)
        map_html = mapa._repr_html_()

        return render_template_string('''
        <!doctype html>
        <html lang="en">
          <head>
            <meta charset="utf-8">
            <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
            <title>Mapa de Localização</title>
          </head>
          <body>
            <div style="width: 100%; height: 100vh;">
              {{ map_html | safe }}
            </div>
          </body>
        </html>
        ''', map_html=map_html)
    else:
        return "Aguardando dados de localização..."

@app.route('/last_message')
def get_last_message():
    if last_location:
        return jsonify({"last_location": last_location})
    else:
        return jsonify({"error": "Nenhuma mensagem recebida ainda"}), 404

if __name__ == '__main__':
    mqtt_thread = threading.Thread(target=run_mqtt)
    mqtt_thread.start()
    app.run(host='0.0.0.0', port=5000)
