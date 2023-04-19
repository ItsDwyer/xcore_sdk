import paho.mqtt.publish as publish
import os

MQTT_SERVER = "Localhost"
MQTT_PORT = 8883

TLS_CERT = "mqtt_broker_certs/client.crt"
TLS_KEY = "mqtt_broker_certs/client.key"
TLS_CA_CERTS ="mqtt_broker_certs/ca.crt"

# test_file = "test_tone_700hz.wav"

# file_path = os.path.abspath(test_file)
file_path = "/home/stevena/Documents/xcore_sdk/examples/freertos/iot/filesystem_support/test_tone_700hz.wav"

with open(file_path, "rb") as f:
    
    file_contents = f.read()

    publish.single("explorer/mics", payload=file_contents, qos=0, hostname=MQTT_SERVER, port=MQTT_PORT, tls={ "ca_certs": TLS_CA_CERTS,"certfile": TLS_CERT,"keyfile": TLS_KEY,})