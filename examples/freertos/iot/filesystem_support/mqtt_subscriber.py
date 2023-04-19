import paho.mqtt.client as mqtt

import ssl

# import configparser

# config = configparser.ConfigParser()
# config.read('../app_config.h') //gave up on this
# hostname = config.get()

def on_message(client, userdata, message):
    print (message.payload) #Print audio data for test

client = mqtt.Client()

client.tls_set(
                ca_certs="mqtt_broker_certs/ca.crt",
                certfile="mqtt_broker_certs/client.crt",
                keyfile="mqtt_broker_certs/client.key",
                cert_reqs=ssl.CERT_NONE,
                tls_version=ssl.PROTOCOL_TLSv1_2
            )

client.on_message = on_message

client.connect("localhost", 8883, 60)

client.subscribe("explorer/mics")

client.loop_forever