import subprocess

from mqtt_config import MQTT_SERVER, MQTT_PORT, MQTT_TOPIC, TLS_CERT, TLS_KEY, TLS_CA_CERTS, NUM_CHANNELS, SAMPLE_WIDTH, SAMPLE_RATE, CHUNK_SIZE

output_file = "output.wav"

# Construct the MQTT command
mqtt_command = f"sudo mosquitto_sub --cafile {TLS_CA_CERTS} --cert {TLS_CERT} --key {TLS_KEY} -v -d -t {MQTT_TOPIC}"

# Construct the SoX command
sox_command = f"sox -t raw -r {SAMPLE_RATE} -b {SAMPLE_WIDTH * 8} -e signed -c {NUM_CHANNELS} - {output_file}"

# Pipe the MQTT command output to the SoX command
process = subprocess.Popen(mqtt_command.split(), stdout=subprocess.PIPE)
output, error = process.communicate()
subprocess.run(sox_command, input=output, shell=True)
