#include <WiFiNINA.h>
#include <Wire.h>
#include <ArtronShop_BH1750.h>

// WiFi credentials
char ssid[] = "Daiyan";
char pass[] = "dairaz1711";

// Variable to track sunlight status
bool Sunlight = false;

// BH1750 light sensor instance
ArtronShop_BH1750 bh1750(0x23, &Wire);

// WiFi client instance
WiFiClient client;

// Host name and paths for IFTTT triggers
char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME = "/trigger/SunlightDetected/json/with/key/bKbnloAsyjWnYJjxEtSCWB";
String PATH2_NAME = "/trigger/SunlightGone/json/with/key/bKbnloAsyjWnYJjxEtSCWB";

// Setup function
void setup() {
  // Initialize WiFi connection
  WiFi.begin(ssid, pass);
  Serial.begin(9600);
  
  // Wait for Serial connection to be established
  while (!Serial);

  // Connect to WiFi
  connectWiFi();

  // Initialize light sensor
  initializeSensor();
}

// Function to connect to WiFi
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

// Function to initialize light sensor
void initializeSensor() {
  Wire.begin();
  Serial.println("Finding Sensor");
  if (!bh1750.begin()) {
    Serial.println("BH1750 not found !");
    delay(1000);
  }
  else {
    Serial.println("Sensor detected");
  }
}

// Function to detect sunlight intensity
float detectSunlight() {
  Serial.print("Light: ");
  float instanceLight = bh1750.light();
  Serial.print(instanceLight);
  Serial.println(" lx");
  return instanceLight;
}

// Function to send message to IFTTT
void sendMessage(String path, String payload) {
  if (!client.connected() && !client.connect(HOST_NAME, 80)) {
    Serial.println("connection failed");
    return;
  }

  // Construct HTTP GET request with payload
  client.print("GET " + path + " HTTP/1.1\r\n");
  client.print("Host: " + String(HOST_NAME) + "\r\n");
  client.print("Connection: close\r\n");
  client.print("\r\n");
  client.print(payload);

  // Wait for response
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  }
  client.stop();
  Serial.println("\ndisconnected");
}

// Main loop
void loop() {
  // Delay before each sensor reading
  delay(1000);
  
  // Read sunlight intensity
  float instanceLight = detectSunlight();
  
  // Delay after reading
  delay(500);

  // Check if sunlight intensity exceeds threshold and sunlight status is false
  if (instanceLight > 200 && !Sunlight) {
    Sunlight = true;
    String payload = "{\"value1\":\"SunlightDetected\"}";
    sendMessage(PATH_NAME, payload);
  }

  // Check if sunlight intensity is within threshold and sunlight status is true
  if (0 < instanceLight && instanceLight <= 200 && Sunlight) {
    Sunlight = false;
    String payload = "{\"value1\":\"SunlightFaded\"}";
    sendMessage(PATH2_NAME, payload);
  }

  Serial.println();
  
  // Delay before next iteration
  delay(60000);
}
