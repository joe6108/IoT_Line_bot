/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <SimpleDHT.h>

int pinDHT11 = 15; //ESP32 GPIO15
SimpleDHT11 dht11;

// Replace with your network credentials
const char* ssid     = ""; //WiFi-名稱
const char* password = "";//WiFi-密碼

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;
String Tem;
String Hum;
// Set Hum and temp
byte temperature = 0;
byte humidity = 0;
int err = SimpleDHTErrSuccess;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var) {
  Serial.println(var);
  // start working...
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
  } else {
    Tem = String(temperature);
    Hum = String(humidity);
  }
  if (var == "STATE") {
    if (digitalRead(ledPin)) {
      ledState = "ON";
    }
    else {
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  if (var == "Temp") {
    return Tem + " C";
  }
  if (var == "Humidy") {
    return Hum + "%";
  }
  return String();
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  //新增，可刪(頭)
  delay(2000);
  File file = SPIFFS.open("/test.txt");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("File Content:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
  //新增，可刪(尾)

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, HIGH);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, LOW);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}

void loop() {
}
