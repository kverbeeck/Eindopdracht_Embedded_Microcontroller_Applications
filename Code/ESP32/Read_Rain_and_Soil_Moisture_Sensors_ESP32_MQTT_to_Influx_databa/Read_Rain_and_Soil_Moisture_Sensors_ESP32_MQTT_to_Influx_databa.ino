#include <WiFi.h>
#include <ESP32Ping.h>
#include <PubSubClient.h>

/*
   Soil Moisture Sensor YL-69 or HL-69
*/
#define soilPin 35
#define red_soil 14 // red soil LED
#define green_soil 12 // green soil LED
// you can adjust the threshold value, 0 - 4095 (12 bits), lower value is wetter.
int soilThresholdValue = 3000;

/*
   Rain Sensor FC-37 or YL-83
*/
#define rainPin 34
#define red_rain 26 // red rain LED
#define green_rain 27 // green rain LED
// you can adjust the threshold value, 0 - 4095 (12 bits), lower value is wetter.
int rainThresholdValue = 2000;

/*
   MQTT client - ESP32
*/
const char* ssid = "BLYNK";
const char* password = "Bucky196";
const char* mqttServer = "192.168.10.124"; // MQTT Broker // KoenRPI4B.local
const int mqttPort = 1883; // MQTT Port
const char* mqttUser = "koen"; // Username Mosquitto
const char* mqttPassword = "k03n123"; // Password Mosquitto
const char* clientID = "ESP32_SOIL"; // MQTT client ID

WiFiClient espClient;
PubSubClient client(espClient);

const IPAddress remote_ip(192, 168, 10, 124); // IP address of MQTT Broker for ping >> KoenRPI4B.local

void setup_wifi() {
  // Delay for 10ms
  delay(10);
  // Print an empty line
  Serial.println();
  // Print to which SSID ESP32 is trying to connect
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  // Initialize Wi-Fi on ESP32 and connect to the configured WLAN
  WiFi.begin(ssid, password);

  // As long as ESP32 is not connected to the configured WLAN
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(F("Trying to Connect"));
  }
  Serial.print(F("\nConnected to the "));
  Serial.print(ssid);
  Serial.println(F(" Wi-Fi network\n"));
}

void RSSI_Info() {
  // Request Wi-Fi Signal Strength 3 times
  for (int i = 0; i < 3; i++) {
    Serial.print(F("RSSI: "));
    Serial.print(WiFi.RSSI());
    Serial.println(F("dBm"));
    delay(1000);
  }
  Serial.println();
}

void ping_MQTT_Broker() {
  // Send 3 pings to the MQTT Broker
  Serial.print(F("Pinging MQTT Broker 3 times: "));
  Serial.println(remote_ip);
  for (int i = 0; i < 3; i++) {
    if (Ping.ping(remote_ip)) {
      Serial.println(F("Success !!"));
    }
    else {
      Serial.println(F("No reply"));
    }
  }
  Serial.println();
}

void DHCP_Info() {
  // Print the ESP32 IP info received from DHCP
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  Serial.print(F("Subnet Mask: "));
  Serial.println(WiFi.subnetMask());
  Serial.print(F("Default Gateway: "));
  Serial.println(WiFi.gatewayIP());
  Serial.println();
}


void setup() {

  Serial.begin(115200);

  /*
    Soil Moisture Sensor YL-69 or HL-69
  */
  pinMode(soilPin, INPUT);
  pinMode(red_soil, OUTPUT);
  pinMode(green_soil, OUTPUT);
  digitalWrite(green_soil, LOW);
  digitalWrite(red_soil, LOW);

  /*
    Rain Sensor FC-37 or YL-83
  */
  pinMode(rainPin, INPUT);
  pinMode(red_rain, OUTPUT);
  pinMode(green_rain, OUTPUT);
  digitalWrite(green_rain, LOW);
  digitalWrite(red_rain, LOW);

  /*
    Initialize MQTT client - ESP32
  */
  setup_wifi();
  RSSI_Info();
  DHCP_Info();
  // ping_MQTT_Broker();
  client.setServer(mqttServer, mqttPort);
}

void loop() {

  /*
    MQTT client - ESP32
  */
  client.loop();

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      Serial.println("connected");
    }
    else {
      /*
          int state ()
          Returns the current state of the client. If a connection attempt fails,
          this can be used to get more information about the failure.

          All of the values have corresponding constants defined in PubSubClient.h.

          Returns:
          -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
          -3 : MQTT_CONNECTION_LOST - the network connection was broken
          -2 : MQTT_CONNECT_FAILED - the network connection failed
          -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
          0 : MQTT_CONNECTED - the client is connected
          1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
          2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
          3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
          4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
          5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
      */
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  /*
    Soil Moiture Sensor YL-69 or HL-69
  */
  // read the input on analog pin 35 (ADC 1 Channel 7):
  int soilVal = analogRead(soilPin);
  Serial.print("Soil Moisture Sensor value is: ");
  Serial.print(soilVal);

  if (soilVal > soilThresholdValue) {
    Serial.println(" - Time to water your plant");
    digitalWrite(green_soil, LOW);
    digitalWrite(red_soil, HIGH);
  }
  else {
    Serial.println(" - Plant doesn't need watering");
    digitalWrite(green_soil, HIGH);
    digitalWrite(red_soil, LOW);
  }

  /*
    Rain Sensor FC-37 or YL-83
  */
  // read the input on analog pin 34 (ADC 1 Channel 6):
  int rainVal = analogRead(rainPin);
  Serial.print("Rain Sensor value is: ");
  Serial.print(rainVal);
  if (rainVal < rainThresholdValue) {
    Serial.println(" - It's raining");
    digitalWrite(green_rain, LOW);
    digitalWrite(red_rain, HIGH);
  }
  else {
    Serial.println(" - It's dry");
    digitalWrite(green_rain, HIGH);
    digitalWrite(red_rain, LOW);
  }

  // Covert integer (soilVal) to char array
  // Must be at least 10 char for the unsigned int (%u) payload
  // home/esp32/dht11_temp b'22.400000'
  char soil_array[10];
  sprintf(soil_array, "%u", soilVal);
  // Covert integer (rainVal) to char array
  // Must be at least 10 char for the unsigned int (%u) payload
  // home/esp32/dht11_humi b'54.000000'
  char rain_array[10];
  sprintf(rain_array, "%u", rainVal);

  // Publish rain and soil sensors data
  client.publish("home/esp32/soil", soil_array);
  client.publish("home/esp32/rain", rain_array);


  // client.disconnect();  // disconnect from the MQTT broker
  // Serial.println("Disconnecting from MQTT...");
  delay(1000 * 5); // Wait for 5 seconds
}
