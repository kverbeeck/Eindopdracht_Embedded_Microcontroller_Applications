# Library to control MQTT
import paho.mqtt.client as mqtt
# Library to control the LEDs
from gpiozero import LED
# Library to control Servo's
from gpiozero import AngularServo

# Temperature
red_temp = LED(27)  # red temp. alarm LED
green_temp = LED(18)  # green temp. status LED

tempThresholdValueHigh = 25.00  # °C
tempThresholdValueLow = 20.00  # °C

# Soil Moisture
# you can adjust the threshold value, 0 - 4095 (12 bits), lower value is wetter.
soilThresholdValue = 3000
servo_soil = AngularServo(22, min_angle=0, max_angle=180)  # Servo(22)

# Rain
# you can adjust the threshold value, 0 - 4095 (12 bits), lower value is wetter.
rainThresholdValue = 2000
servo_rain = AngularServo(23, min_angle=0, max_angle=180)  # Servo(23)

# Greenhouse Dynamic Roof
yellow_roof = LED(21)  # Dynamic Roof open is on, closed is off.

# Heating
red_heating = LED(5)  # Heating on is on, heating off is off.

# Sprinklers
blue_sprinklers = LED(26)  # Sprinklers on is on, sprinklers off, is off.


# The callback for when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    # Print result of connection attempt
    # Connection Return Codes:
    # 0: Connection successful
    # 1: Connection refused – incorrect protocol version
    # 2: Connection refused – invalid client identifier
    # 3: Connection refused – server unavailable
    # 4: Connection refused – bad username or password
    # 5: Connection refused – not authorised
    # 6-255: Currently unused.
    if rc == 0:
        print("\nConnection is good \nReturned code =", rc)
    else:
        print("Bad connection Returned code= ", rc)
    # Subscribe to the topics by using a wild card,
    # receive any messages published in it.
    client.subscribe("home/esp32/#")


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    # Without the command below, the message will be decoded in Bytes ('bXXX'),
    # so we need to convert it to UTF-8 before using it.
    msg.payload = msg.payload.decode("utf-8")
    # Print a received msg
    print("Message received >>> " + msg.topic + " >>> " + str(msg.payload))
    print("Topic is: " + msg.topic)
    print("Payload is: " + msg.payload)

    # Temperature Control
    if msg.topic == "home/esp32/dht11_temp":
        if float(msg.payload) > tempThresholdValueHigh:
            print(" - To hot, Opening Greenhouse Roof")
            green_temp.off()
            red_temp.on()
            yellow_roof.on()
            red_heating.off()
        elif float(msg.payload) < tempThresholdValueLow:
            print(" - To cold, Greenhouse Heating On")
            green_temp.off()
            red_temp.on()
            yellow_roof.off()
            red_heating.on()
        else:
            print(" - Temperature is OK")
            green_temp.on()
            red_temp.off()
            yellow_roof.off()
            red_heating.off()

    # Soil Control
    if msg.topic == "home/esp32/soil":
        if int(msg.payload) > soilThresholdValue:
            print(" - Watering your plant")
            servo_soil.angle = 0
            blue_sprinklers.on()
        else:
            print(" - Plants don't need watering")
            servo_soil.angle = 180
            blue_sprinklers.off()

    # Rain Control
    if msg.topic == "home/esp32/rain":
        if int(msg.payload) < rainThresholdValue:
            print(" - It's raining")
            servo_rain.angle = 180
        else:
            print(" - It's dry")
            servo_rain.angle = 0

# broker is located on this machine (loopback address)
broker = "127.0.0.1"
# Create instance of client
# !!!!! with client ID “digi_mqtt_test” !!!!
client = mqtt.Client()
# Define callback function for successful connection
client.on_connect = on_connect
# Define callback function for receipt of a message
client.on_message = on_message
# Define username and password stored in Broker password file
client.username_pw_set("koen", "k03n123")
# Connect to (broker, port, keepalive-time)
client.connect(broker, 1883, 60)

# The Topics and payloads, you would like to publish
# Should be located before "client.loop_forever()"

client.publish("home/RPi4B/Lighting", 1)  # publish

# Start networking daemon
client.loop_forever(1)
