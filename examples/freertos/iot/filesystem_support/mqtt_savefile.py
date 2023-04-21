import subprocess

from mqtt_config import MQTT_SERVER, MQTT_PORT, MQTT_TOPIC, TLS_CERT, TLS_KEY, TLS_CA_CERTS, NUM_CHANNELS, SAMPLE_WIDTH, SAMPLE_RATE, CHUNK_SIZE

# MQTT parameters
cafile = "mqtt_broker_certs/ca.crt"
certfile = "mqtt_broker_certs/client.crt"
keyfile = "mqtt_broker_certs/client.key"
topic = "explorer/mics"

# Audio parameters
rate = 16000
bits = 16
encoding = "signed-integer"
channels = 1
output_file = "output.wav"

# Construct the MQTT command
mqtt_command = f"sudo mosquitto_sub --cafile {cafile} --cert {certfile} --key {keyfile} -v -d -t {topic}"

# Construct the SoX command
sox_command = f"sox -t raw -r {rate} -b {bits} -e {encoding} -c {channels} - {output_file}"

# Pipe the MQTT command output to the SoX command
process = subprocess.Popen(mqtt_command.split(), stdout=subprocess.PIPE)
output, error = process.communicate()
subprocess.run(sox_command, input=output, shell=True)
