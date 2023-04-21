import paho.mqtt.client as mqtt
import subprocess
import io

from mqtt_config import MQTT_SERVER, MQTT_PORT, MQTT_TOPIC, TLS_CERT, TLS_KEY, TLS_CA_CERTS, NUM_CHANNELS, SAMPLE_WIDTH, SAMPLE_RATE, CHUNK_SIZE

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    print("Received audio stream from topic: " + msg.topic)
    # Use sox command to play the received audio
    play_process = subprocess.Popen(["sox", "-t", "raw", "-r", str(SAMPLE_RATE), "-e", "signed", "-b", str(SAMPLE_WIDTH * 8), "-c", str(NUM_CHANNELS), "-L", "-", "-d"], stdin=subprocess.PIPE)
    buffer = io.BytesIO()
    buffer.write(msg.payload)
    buffer.seek(0)
    play_process.communicate(input=buffer.read())

client = mqtt.Client()
client.tls_set(TLS_CA_CERTS, certfile=TLS_CERT, keyfile=TLS_KEY)
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_SERVER, MQTT_PORT, 60)
client.loop_forever()
