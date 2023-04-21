
import paho.mqtt.client as mqtt
import subprocess

from mqtt_config import MQTT_SERVER, MQTT_PORT, MQTT_TOPIC, TLS_CERT, TLS_KEY, TLS_CA_CERTS, NUM_CHANNELS, SAMPLE_WIDTH, SAMPLE_RATE, CHUNK_SIZE

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    print("Received audio stream from topic: " + msg.topic)
    # Use sox command to play the received audio
    play_process = subprocess.Popen(["sox", "-t", "raw", "-r", str(SAMPLE_RATE), "-e", "signed", "-b", str(SAMPLE_WIDTH * 8), "-c", str(NUM_CHANNELS), "-L", "-", "-d"], stdin=subprocess.PIPE)
    play_process.communicate(input=msg.payload)

client = mqtt.Client()
client.tls_set(TLS_CA_CERTS, certfile=TLS_CERT, keyfile=TLS_KEY)
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_SERVER, MQTT_PORT, 60)
client.loop_forever()


# import paho.mqtt.client as mqtt
# import pyaudio
# import wave

# MQTT_SERVER = "localhost"
# MQTT_PORT = 8883

# TLS_CERT = "mqtt_broker_certs/client.crt"
# TLS_KEY = "mqtt_broker_certs/client.key"
# TLS_CA_CERTS = "mqtt_broker_certs/ca.crt"

# CHANNELS = 1
# SAMPLE_WIDTH = 2
# SAMPLE_RATE = 16000
# CHUNK_SIZE = 1024

# audio_player = pyaudio.PyAudio()

# def on_connect(client, userdata, flags, rc):
#     print("Connected with result code "+str(rc))
#     client.subscribe("explorer/mics")

# def on_message(client, userdata, msg):
#     print("Received message on topic "+msg.topic)

#     # Create a new PyAudio stream to play the received audio
#     stream = audio_player.open(format=audio_player.get_format_from_width(SAMPLE_WIDTH),
#                     channels=CHANNELS,
#                     rate=SAMPLE_RATE,
#                     output=True)

#     # Write the audio data to the stream in chunks
#     stream.write(msg.payload)

#     # Stop the stream and terminate the PyAudio instance
#     stream.stop_stream()
#     stream.close()

# client = mqtt.Client()
# client.tls_set(ca_certs=TLS_CA_CERTS, certfile=TLS_CERT, keyfile=TLS_KEY)
# client.on_connect = on_connect
# client.on_message = on_message

# client.connect(MQTT_SERVER, MQTT_PORT, 60)

# client.loop_forever()

# audio_player.terminate()
