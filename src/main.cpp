/**
   Wifi temp reader

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>


#define ROOM "gangen"

#define DHTPIN D4     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

ESP8266WiFiMulti WiFiMulti;


void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println("DHT11!");

  for (uint8_t t = 0; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFiMulti.addAP("Nordheim", "MasseyFerguson135");
  Serial.print("Connecting to Nordheim.");
  for (int i = 0; WiFiMulti.run() != WL_CONNECTED && i < 14; i++) { // Give 7 secs
    Serial.print(".");
    delay(500);
  }

  if (WiFiMulti.run() != WL_CONNECTED) {
    Serial.println("Could not connect to Nordheim, trying Ikkefaen.");

    WiFiMulti.addAP("Ikkefaen");
    while (WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
  }

  IPAddress ip = WiFi.localIP();
  Serial.printf("\n[WIFI] IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

  dht.begin();
}


String createTemperatureUrl(String room, String temp, String humidity) {
  //String s = "https://data.sparkfun.com/input/OG5GQM0payiEM1M72g1O?private_key=8bMb6Kxp8qs2dRd4neRa&room=%s&temp=%f\n";
  String s = "http://dweet.io/dweet/for/nordheim-%s?temperature=%f&humidity=%g";
  s.replace("%s", room);
  s.replace("%f", temp);
  s.replace("%g", humidity);

  Serial.printf(s.c_str());
  Serial.println();

  return s;
}


void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %, ");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C ");

    String url = createTemperatureUrl(ROOM, String(t), String(h));

    http.begin(url);

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode >= 200) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %d\n", httpCode);
    }

    http.end();
  }

  delay(30 * 60 * 1000);
}


