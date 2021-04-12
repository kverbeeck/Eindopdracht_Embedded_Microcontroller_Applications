# Library to control MQTT
import paho.mqtt.client as mqtt
# The Capacitor will block the flow of current to ground when
# fully charged !!!
from gpiozero import LightSensor

from time import sleep

# GPIO.add_event_detect(PIN, GPIO.BOTH, handle) is generating error:
# RuntimeError: Failed to add edge detection
#
# Solution: This issue is because an unknown conflict using Pin7 (GPIO4)
# which appears to have a secondary function of GCLK. Moving it to GPIO13,
# which has no secondary functions allows it to work,
sensor = LightSensor(13)

# Define Variables
# broker is located on this machine (loopback address)
MQTT_BROKER = "127.0.0.1"
MQTT_PORT = 1883
MQTT_KEEPALIVE_INTERVAL = 60
MQTT_TOPIC = "helloTopic"
MQTT_MSG = "hello MQTT"

# Define on_publish event function
def on_publish(client, userdata, mid):
    print("Message Published...")

while True:
    # Create instance of client
    client = mqtt.Client()
    # Define username and password stored in Broker password file
    client.username_pw_set("koen", "k03n123")
    # Register publish callback function
    client.on_publish = on_publish
    # Connect to (broker, port, keepalive-time)
    client.connect(MQTT_BROKER, MQTT_PORT, MQTT_KEEPALIVE_INTERVAL)

    sensorVal = sensor.value
    print(sensorVal)

    if float(sensorVal < 0.7):
        print("It's Dark")
        client.publish("home/RPi4B/Lighting", 1)  # publish
    elif float(sensorVal >= 0.7):
        print("It's Light")
        client.publish("home/RPi4B/Lighting", 0)  # publish

    sleep(1) # Depends on keep-alive time?
    # Disconnect from MQTT_Broker
    client.disconnect()
