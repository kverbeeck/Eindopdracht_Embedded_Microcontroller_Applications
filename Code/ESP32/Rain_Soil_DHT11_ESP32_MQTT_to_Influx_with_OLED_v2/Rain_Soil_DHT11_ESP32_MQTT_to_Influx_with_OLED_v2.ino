// Include the necessary libraries
#include <DHT.h> // DHT11 sensor 
#include <WiFi.h> // Wi-Fi Client
#include <ESP32Ping.h> // Ping Client for testing
#include <PubSubClient.h> // MQTT Client
#include <Adafruit_GFX.h> // Supporting library for OLED screen
#include <Adafruit_SSD1306.h> // OLED screen
#include <Wire.h> // I2C bus

/*
   Lighthing
*/
#define whiteLight 4

/*
   DHT11 Sensor (Temperature and Humidity)
*/
#define DHTPIN 26  // for ESP32
#define DHTTYPE DHT11  // DHT11 or DHT22

DHT dht(DHTPIN, DHTTYPE);

/*
   Soil Moisture Sensor YL-69 or HL-69
*/
#define soilPin 35
// you can adjust the threshold value, 0 - 4095 (12 bits), lower value is wetter.
int soilThresholdValue = 3000;

/*
   Rain Sensor FC-37 or YL-83
*/
#define rainPin 34
// you can adjust the threshold value, 0 - 4095 (12 bits), lower value is wetter.
int rainThresholdValue = 2000;

/* OLED 128x64 Monochrone Screen
*/
#define SCREEN_WIDTH 128 // Width of the screen
#define SCREEN_HEIGHT 64 // Height of the screen

// Initialize the OLED screen
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// BITMAP Koen_LEGO start-up screen
static const uint8_t image_data_KoenLego2[1024] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xbf, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x3f, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x3f, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x3e, 0x7e, 0x7b, 0x3f, 0xff, 0xff,
  0xff, 0xfe, 0x03, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf2, 0x78, 0x18, 0x38, 0x1f, 0xff, 0xff,
  0xff, 0xff, 0xf3, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x71, 0x99, 0x98, 0x9f, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x73, 0x99, 0x99, 0x9f, 0xff, 0xff,
  0xff, 0xff, 0xd7, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf2, 0x73, 0x90, 0x19, 0x9f, 0xff, 0xff,
  0xff, 0xff, 0x97, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x33, 0x91, 0xf9, 0x9f, 0xff, 0xff,
  0xff, 0xff, 0x87, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x13, 0x99, 0xf9, 0x9f, 0xff, 0xff,
  0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x98, 0x18, 0x19, 0x9f, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x88, 0x3c, 0x19, 0x9f, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xef, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xf8, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf7, 0xc1, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf8, 0x00, 0x1f, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xfc, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xfe, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x00, 0x07, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xe7, 0xff,
  0xf0, 0x00, 0x8e, 0x00, 0xff, 0xff, 0xff, 0xfc, 0xf9, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xe7, 0xff,
  0x80, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xc3, 0xfc, 0xf9, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xe7, 0xff,
  0x03, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x03, 0xfe, 0x73, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xe7, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x03, 0xfe, 0x73, 0x0e, 0x18, 0x3c, 0x3c, 0x3c, 0x66, 0x7f,
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x01, 0xfe, 0x72, 0x06, 0x18, 0x18, 0x18, 0x18, 0x24, 0x7f,
  0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x01, 0xfe, 0x26, 0x66, 0x39, 0x99, 0x9b, 0x91, 0xe4, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x01, 0xff, 0x26, 0x06, 0x7b, 0x98, 0x10, 0x13, 0xe1, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x26, 0x06, 0x7b, 0x90, 0x10, 0x13, 0xe1, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xff, 0x06, 0x7e, 0x79, 0x99, 0xf3, 0xf3, 0xe4, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0xff, 0x8e, 0x7e, 0x79, 0x99, 0xf9, 0xf1, 0x24, 0x7f,
  0x00, 0x00, 0x00, 0x18, 0x03, 0xc0, 0x00, 0xff, 0x8f, 0x06, 0x70, 0x18, 0x18, 0x18, 0x26, 0x7f,
  0x00, 0x00, 0x00, 0x20, 0x01, 0xc6, 0x00, 0x7f, 0x9f, 0x8e, 0x7a, 0x7e, 0x3c, 0x3c, 0x67, 0x7f,
  0x20, 0x00, 0x00, 0x28, 0x01, 0xc6, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x60, 0x00, 0x00, 0x00, 0x00, 0x60, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xe3, 0x00, 0x40, 0x00, 0x00, 0xe0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf3, 0x00, 0x00, 0x00, 0x0b, 0x10, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0x00, 0x00, 0x00, 0x0c, 0x10, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf8, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xfc, 0x10, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xe0, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xfb, 0xc0, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf8, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xe0, 0x80, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xc1, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x82, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x04, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/*
   MQTT client - ESP32
*/
const char* ssid = "BLYNK";
const char* password = "Bucky196";

const char* lighting_topic = "home/RPi4B/Lighting"; // MQTT Topic

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

// Callback function when an MQTT message is received
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  //Serial.print("Message:");
  //for (int i = 0; i < length; i++) {
  //  Serial.print((char)payload[i]);
  //}

  Serial.print((char)payload[0]);

  if ((char)payload[0] == '1') {
    digitalWrite(whiteLight, HIGH);
  }
  else {
    digitalWrite(whiteLight, LOW);
  }

  Serial.println();
  Serial.println("-----------------------");
}

//------------------------------------------------------------------------------------------------------------------------

void setup() {

  Serial.begin(115200);

  /*
    Lighthing
  */
  pinMode(whiteLight, OUTPUT);

  /* OLED 128x64 Monochrone Screen
  */
  // Initialize the OLED screen on I2C address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000); // Pause for 2 seconds

  // Clear the buffer.
  display.clearDisplay();

  // Draw Start-up screen bitmap on the screen
  // const tImage KoenLego2 = { image_data_KoenLego2, 128, 64, 8 }; >> info from bitmap (LCD Image Converter)
  display.drawBitmap(0, 0, image_data_KoenLego2, 128, 64, 1);
  display.display();

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
    DHT11 Sensor (Temperature and Humidity)
  */
  dht.begin();

  /*
    Initialize MQTT client - ESP32
  */
  setup_wifi();
  RSSI_Info();
  DHCP_Info();
  // ping_MQTT_Broker();
  client.setServer(mqttServer, mqttPort);

  // set a callback function for when data is received from broker
  client.setCallback(callback);

  // connect to broker
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
  // subscribe to topic
  client.subscribe(lighting_topic);
}

void loop() {

  /*
    MQTT client - ESP32
  */
  client.loop();

  /*
    DHT11 Sensor (Temperature and Humidity)
  */
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // check if returns are valid and print the sensor data
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(temperature - 2); // Temperature correction
    Serial.println("°C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  // Covert float (temperature) to char array
  // Must be at least 10 char for the float payload
  // home/esp32/dht11_temp b'22.400000'
  char temp_array[10];
  sprintf(temp_array, "%f", temperature - 2); // Temperature correction
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

  /*
    Soil Moiture Sensor YL-69 or HL-69
  */
  // read the input on analog pin 35 (ADC 1 Channel 7):
  int soilVal = analogRead(soilPin);
  Serial.print("Soil Moisture Sensor value is: ");
  Serial.print(soilVal);

  if (soilVal > soilThresholdValue) {
    Serial.println(" - Time to water your plant");
  }
  else {
    Serial.println(" - Plant doesn't need watering");
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
  }
  else {
    Serial.println(" - It's dry");
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

  /*
   * Display the data on the OLED screen
   */
  // Clear the OLED buffer
  display.clearDisplay();
  // Set the text color to white, 
  // failing to set this wil give you a black screen
  display.setTextColor(WHITE);
  display.invertDisplay(false);
  // display current temperature in °C
  display.setFont();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Curr. Temp.: ");
  display.print(temperature - 2); // Read temperature minus correction
  display.print(" ");
  display.cp437(true);
  display.write(167);
  display.print("C");
  // display current humidity in %
  display.setCursor(0, 10);
  display.print("Curr. Hum:");
  display.print(humidity); 
  display.print(" ");
  display.print("%");
  // display current status Soil Moisture Sensor
  display.setCursor(0, 20);
  display.print("Status Soil Sensor:");
  display.setCursor(0, 30);
  display.print(soilVal); 
  display.print(" ");
  if (soilVal > soilThresholdValue) {
    display.print("Soil is Dry");
  }
  else {
    display.print("Soil is wet");
  }
  // display current status Rain Sensor
  display.setCursor(0, 40);
  display.print("Status Rain Sensor:");
  display.setCursor(0, 50);
  display.print(rainVal); 
  display.print(" ");
  if (rainVal < rainThresholdValue) {
    display.print("It's raining");
  }
  else {
    display.print("It's dry");
  }
  

  // "display.display()" is NOT necessary after every single drawing command,
  // you can batch up a bunch of drawing operations and then update the screen
  // all at once by calling "display.display()".
  display.display();

  // client.disconnect();  // disconnect from the MQTT broker
  // Serial.println("Disconnecting from MQTT...");
  delay(1000 * 1); // Wait for 2 seconds
}
