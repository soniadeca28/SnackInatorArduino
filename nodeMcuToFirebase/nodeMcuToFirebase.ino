#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Set these to run example.
#define FIREBASE_HOST "snackinator-lic-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "7vNU6vUl9QqcWM8b0JvpWGjqo9himE8jAguDNAz3"
#define WIFI_SSID "S&D"
#define WIFI_PASSWORD "budinca123"

const long utcOffsetInSeconds = 10800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(9600);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("connected!");
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  timeClient.begin();
}

void loop() {

  timeClient.update();

  //set hour
  Firebase.setInt("hour",timeClient.getHours());

  //set minutes
  Firebase.setInt("minutes",timeClient.getMinutes());
  
  delay(100);
}
