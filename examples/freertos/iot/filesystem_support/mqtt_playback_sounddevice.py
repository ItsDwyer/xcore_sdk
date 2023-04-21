import paho.mqtt.client as mqtt
import sounddevice as sd
import numpy as np
import threading

from mqtt_config import MQTT_SERVER, MQTT_PORT, MQTT_TOPIC, TLS_CERT, TLS_KEY, TLS_CA_CERTS, NUM_CHANNELS, SAMPLE_WIDTH, SAMPLE_RATE, CHUNK_SIZE


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(MQTT_TOPIC)

def playback_thread(audio_data):
    with sd.OutputStream(channels=NUM_CHANNELS, blocksize=CHUNK_SIZE, dtype='float32') as stream:
        while True:
            stream.write(audio_data.astype('float32'))



def on_message(client, userdata, msg):
    print("Received audio stream from topic: " + msg.topic)
    # Convert raw audio data to a numpy array
    audio_data = np.frombuffer(msg.payload, dtype=np.int16)
    # Start a new thread to play the audio using sounddevice
    t = threading.Thread(target=playback_thread, args=(audio_data,))
    t.start()

client = mqtt.Client()
client.tls_set(TLS_CA_CERTS, certfile=TLS_CERT, keyfile=TLS_KEY)
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_SERVER, MQTT_PORT, 60)
client.loop_forever()




# OLD WAY WITHOUT QUEUING
# import paho.mqtt.client as mqtt
# import sounddevice as sd
# import numpy as np

# from mqtt_config import MQTT_SERVER, MQTT_PORT, MQTT_TOPIC, TLS_CERT, TLS_KEY, TLS_CA_CERTS, NUM_CHANNELS, SAMPLE_WIDTH, SAMPLE_RATE, CHUNK_SIZE


# def on_connect(client, userdata, flags, rc):
#     print("Connected with result code "+str(rc))
#     client.subscribe(MQTT_TOPIC)

# def on_message(client, userdata, msg):
#     print("Received audio stream from topic: " + msg.topic)
#     # Convert raw audio data to a numpy array
#     audio_data = np.frombuffer(msg.payload, dtype=np.int16)
#     # Play the audio using sounddevice
#     sd.play(audio_data, SAMPLE_RATE, NUM_CHANNELS)

# client = mqtt.Client()
# client.tls_set(TLS_CA_CERTS, certfile=TLS_CERT, keyfile=TLS_KEY)
# client.on_connect = on_connect
# client.on_message = on_message

# client.connect(MQTT_SERVER, MQTT_PORT, 60)
# client.loop_forever()
