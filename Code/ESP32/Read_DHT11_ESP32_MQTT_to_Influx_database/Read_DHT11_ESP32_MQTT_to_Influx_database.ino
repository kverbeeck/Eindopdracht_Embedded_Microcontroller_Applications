#include <DHT.h>
#include <WiFi.h>
#include <ESP32Ping.h>
#include <PubSubClient.h>


/*
   DHT11 Sensor (Temperature and Humidity)
*/
#define DHTPIN 4  // for ESP32
#define DHTTYPE DHT11  // DHT11 or DHT22

DHT dht(DHTPIN, DHTTYPE);


/*
   MQTT client - ESP32
*/
const char* ssid = "BLYNK";
const char* password = "Bucky196";
const char* mqttServer = "192.168.10.124"; // MQTT Broker // KoenRPI4B.local
const int mqttPort = 1883; // MQTT Port
const char* mqttUser = "koen"; // Username Mosquitto
const char* mqttPassword = "k03n123"; // Password Mosquitto
const char* clientID = "ESP32_DHT11"; // MQTT client ID

WiFiClient espClient;
PubSubClient client(espClient);

const IPAddress remote_ip(192, 168, 10, 124); // IP address of MQTT Broker // KoenRPI4B.local

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
    DHT11 Sensor (Temperature and Humidity)
  */
  dht.begin();

  /*
    MQTT client - ESP32
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

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // check if returns are valid and print the sensor data
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  // Covert float (temperature) to char array
  // Must be at least 10 char for the float payload
  // home/esp32/dht11_temp b'22.400000'
  char temp_array[10];
  sprintf(temp_array, "%f", temperature);
  // Covert float (humidity) to char array
  // Must be at least 10 char for the float payload
  // home/esp32/dht11_humi b'54.000000'
  char humi_array[10];
  sprintf(humi_array, "%f", humidity);

  // Only publish data if the sensor is sending actual data (connected) 
  if (not(isnan(temperature) || isnan(humidity))) {
    client.publish("home/esp32/dht11_temp", temp_array);
    client.publish("home/esp32/dht11_humi", humi_array);
  }
  
  client.disconnect();  // disconnect from the MQTT broker
  Serial.println("Disconnecting from MQTT...");
  delay(1000 * 5); // Wait for 5 seconds
}
