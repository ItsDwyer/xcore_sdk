import paho.mqtt.publish as publish
import os

from mqtt_config import MQTT_SERVER, MQTT_PORT, MQTT_TOPIC, TLS_CERT, TLS_KEY, TLS_CA_CERTS, NUM_CHANNELS, SAMPLE_WIDTH, SAMPLE_RATE, CHUNK_SIZE

MQTT_SERVER = "Localhost"
MQTT_PORT = 8883

TLS_CERT = "mqtt_broker_certs/client.crt"
TLS_KEY = "mqtt_broker_certs/client.key"
TLS_CA_CERTS ="mqtt_broker_certs/ca.crt"

file_path = "test_tone_700hz.wav"

with open(file_path, "rb") as f:
    
    file_contents = f.read()
    print(f"Publishing entire wav file")
    publish.single("explorer/mics", payload=file_contents, qos=1, hostname=MQTT_SERVER, port=MQTT_PORT, tls={ "ca_certs": TLS_CA_CERTS,"certfile": TLS_CERT,"keyfile": TLS_KEY,})