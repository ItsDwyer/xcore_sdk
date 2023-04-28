import paho.mqtt.publish as publish
import wave
import struct
import time

from mqtt_config import MQTT_SERVER, MQTT_PORT, MQTT_TOPIC, TLS_CERT, TLS_KEY, TLS_CA_CERTS, NUM_CHANNELS, SAMPLE_WIDTH, SAMPLE_RATE, CHUNK_SIZE

def publish_audio_stream(filepath, mqtt_topic):
    with wave.open(filepath, 'rb') as wave_file:
        # #DONT NEED WAV HEADER AT ALL!
        # num_channels = wave_file.getnchannels()
        # sample_width = wave_file.getsampwidth()
        # sample_rate = wave_file.getframerate()
        # total_frames = wave_file.getnframes()
        
        # wav_header = b'RIFF'
        # wav_header += struct.pack('<I', 36 + (total_frames * sample_width * num_channels))
        # wav_header += b'WAVE'
        # wav_header += b'fmt '
        # wav_header += struct.pack('<IHHIIHH', 16, 1, num_channels, sample_rate, sample_rate * sample_width * num_channels, sample_width * num_channels, sample_width * 8)
        # wav_header += b'data'
        # wav_header += struct.pack('<I', total_frames * sample_width * num_channels)
        
        # publish.single(mqtt_topic, payload=wav_header, qos=2, hostname=MQTT_SERVER, port=MQTT_PORT, tls={
        #                "ca_certs": TLS_CA_CERTS, "certfile": TLS_CERT, "keyfile": TLS_KEY})
        
        with open(filepath, "rb") as f:
            while True:
                chunk = f.read(CHUNK_SIZE)
                if not chunk:
                    break
                publish.single(mqtt_topic, payload=chunk, qos=2, hostname=MQTT_SERVER, port=MQTT_PORT, tls={
                       "ca_certs": TLS_CA_CERTS, "certfile": TLS_CERT, "keyfile": TLS_KEY})

print(f"Publishing wav as a continuous audio stream")
publish_audio_stream("test_tone_700hz.wav", MQTT_TOPIC)



# WITHOUT RIFF HEADER
# import paho.mqtt.publish as publish
# import wave
# import time

# from mqtt_config import MQTT_SERVER, MQTT_PORT, MQTT_TOPIC, TLS_CERT, TLS_KEY, TLS_CA_CERTS, NUM_CHANNELS, SAMPLE_WIDTH, SAMPLE_RATE, CHUNK_SIZE

# print(f"Publishing test file as a continuous audio stream")

# with open("test_tone_700hz.wav", "rb") as f:
#     while True:
#         chunk = f.read(CHUNK_SIZE)
#         if not chunk:
#             break
#         publish.single(MQTT_TOPIC, payload=chunk, qos=2, hostname=MQTT_SERVER, port=MQTT_PORT, tls={
#                        "ca_certs": TLS_CA_CERTS, "certfile": TLS_CERT, "keyfile": TLS_KEY})
    

    #_________________________________
    # while True:
    #     wf.rewind()
    #     for i in range(0, nframes, CHUNK_SIZE):
    #         data = wf.readframes(CHUNK_SIZE)
    #         publish.single(MQTT_TOPIC, payload=data, qos=1, hostname=MQTT_SERVER, port=MQTT_PORT, tls={
    #                "ca_certs": TLS_CA_CERTS, "certfile": TLS_CERT, "keyfile": TLS_KEY,}, max_payload=1000000)
    #         time.sleep(0.01)

# sample_rate = 44100
# frequency = 700
# duration = 5

# data = b''

# for i in range(int(sample_rate * duration)):
#     sample = 0.5 * (math.sin(2 * math.pi * frequency * i / sample_rate))
#     sample = max(min(sample, 0.9999), -0.9999)  # Clip sample to range [-0.9999, 0.9999]
#     data += struct.pack('h', int(sample * 32767))

# publish.single("explorer/mics", payload=data, qos=1, hostname=MQTT_SERVER, port=MQTT_PORT, tls={
#                    "ca_certs": TLS_CA_CERTS, "certfile": TLS_CERT, "keyfile": TLS_KEY})


# import pyaudio
# import paho.mqtt.publish as publish
# import time

# MQTT_SERVER = "localhost"
# MQTT_PORT = 8883

# TLS_CERT = "mqtt_broker_certs/client.crt"
# TLS_KEY = "mqtt_broker_certs/client.key"
# TLS_CA_CERTS = "mqtt_broker_certs/ca.crt"

# FORMAT = pyaudio.paInt16
# CHANNELS = 1
# RATE = 16000
# CHUNK_SIZE = 1024

# audio = pyaudio.PyAudio()

# stream = audio.open(format=FORMAT, channels=CHANNELS, rate=RATE, input=True, frames_per_buffer=CHUNK_SIZE)

# while True:
#     # Read audio data from the microphone
#     data = stream.read(CHUNK_SIZE)

#     # Publish the audio data to MQTT
#     publish.single("explorer/mics", payload=data, qos=1, hostname=MQTT_SERVER, port=MQTT_PORT, tls={
#                    "ca_certs": TLS_CA_CERTS, "certfile": TLS_CERT, "keyfile": TLS_KEY})

#     # Wait for a short period of time before publishing the next chunk
#     time.sleep(0.1)

# # Close the audio stream and terminate PyAudio
# stream.stop_stream()
# stream.close()
# audio.terminate()
