#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>

// --- WiFi ---
const char* ssid = "iPhone";
const char* password = "123456789";

// --- ThingSpeak ---
const char* thingSpeakAddress = "http://api.thingspeak.com/update";
String writeAPIKey = "ANDEVP3DHNVPU5ZQ"; // from your ThingSpeak channel

// --- Sensors ---
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Initialize I2C and BMP180
  Wire.begin(D2, D1);
  if (!bmp.begin()) {
    Serial.println("BMP180 not detected!");
    while (1);
  }

  dht.begin();
  Serial.println("Sensors initialized.");
}

void loop() {
  // --- Read sensors ---
  float tempDHT = dht.readTemperature();
  float humidity = dht.readHumidity();
  float tempBMP = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100; // hPa

  Serial.print("Temp(DHT): "); Serial.print(tempDHT); Serial.print(" °C\t");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.print(" %\t");
  Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" hPa");

  // --- Send to ThingSpeak ---
  if (WiFi.status() == WL_CONNECTED) {
    String url = String(thingSpeakAddress) + "?api_key=" + writeAPIKey +
                 "&field1=" + String(tempDHT) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(pressure);

    HTTPClient http;
    http.begin(client, url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("Data sent to ThingSpeak.");
    } else {
      Serial.println("Error sending data to ThingSpeak.");
    }
    http.end();
  }

  delay(20000); // ThingSpeak allows 15-second updates minimum
}