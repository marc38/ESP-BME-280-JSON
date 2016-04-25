// Now using ESP8266.....
// Sample Arduino Json Web Server
// Created by Benoit Blanchon.
// Modified by Marc MONTEILLER, including BM280 sensor
// Heavily inspired by "Web Server" from David A. Mellis and Tom Igoe

// include :

/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C don't need to declare pins out

// needed to avoid link error on ram check
extern "C" 
{
#include "user_interface.h"
}
ADC_MODE(ADC_VCC); // use for internal checking voltage

WiFiServer server(80);
WiFiClient client;
const char* ssid = "your ssid";
const char* password = "your password";
float pfPress,pfHum,pfTemp,pfVcc;
bool readRequest(WiFiClient& client) {
  bool currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n' && currentLineIsBlank) {
        return true;
      } else if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }
  return false;
}

/* prepare JSON data as example  :

{
  "temp": [
    24.37
  ],
  "hum": [
    44.86
  ],
  "press": [
    988.30
  ],
  "volt": [
    3.474
  ]
}

*/

JsonObject& prepareResponse(JsonBuffer& jsonBuffer) {
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& tempValues = root.createNestedArray("temp");
    tempValues.add(pfTemp);
  JsonArray& humiValues = root.createNestedArray("hum");
    humiValues.add(pfHum);
  JsonArray& pressValues = root.createNestedArray("press");
    pressValues.add(pfPress);
  JsonArray& EsPvValues = root.createNestedArray("volt");
    EsPvValues.add(pfVcc/1000, 3);   
  return root;
}

void writeResponse(WiFiClient& client, JsonObject& json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Access-Control-Allow-Origin: *");   
  client.println("Access-Control-Allow-Methods: GET");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();

  json.prettyPrintTo(client);
}

void setup() {
delay(2000);
  bme.begin();
  // inital connect
  WiFi.mode(WIFI_STA);
  delay(1000);
    // Connect to WiFi network
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    bool success = readRequest(client);
    if (success) {
         delay(500);
   pfTemp = bme.readTemperature();   
   pfHum = bme.readHumidity();
   pfPress = bme.readPressure() / 100.0F;
   delay(500);
   pfVcc = ESP.getVcc();
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& json = prepareResponse(jsonBuffer);
      writeResponse(client, json);
    }
    delay(1);
    client.stop();
  }
}
