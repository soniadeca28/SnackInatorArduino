#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>

#define code "SNK2"

// Set these to run example.
#define FIREBASE_HOST "snackinator-lic-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "7vNU6vUl9QqcWM8b0JvpWGjqo9himE8jAguDNAz3"
#define WIFI_SSID "S&D"
#define WIFI_PASSWORD "budinca123"

const long utcOffsetInSeconds = 10800; //set time zone
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

SoftwareSerial softSerial(D6, D5);

char firebasePath[] = "SnackInators/SNK1/dataFromApp/breakfastHour";
char * path;
char * breakfastHour;
char * breakfastMinute;
char * lunchHour;
char * lunchMinute;
char * dinnerHour;
char * dinnerMinute;

char * servingBreakfast;
char * servingLunch;
char * servingDinner;

char * fountainAllDay;

bool sent = false;

String BREAKFAST = "";
String LUNCH = "";
String DINNER = "";

String FOUNTAIN = "";
int status;
int previousStatus = 1;

String notification;
char *pathSendTo;
String notificationMessage;

String distancePath;
String waterNotifPath;
String weightPath;

int currentTime, lastTimeOfNotificationFood, lastTimeOfNotificationWater;

void getFirebasePaths() {
  path = (char * ) malloc(255);
  strcpy(path, "SnackInators/");
  strcat(path, code);
  strcat(path, "/dataFromApp");

  pathSendTo = (char * ) malloc(255);
  strcpy(pathSendTo, "SnackInators/");
  strcat(pathSendTo, code);
  strcat(pathSendTo, "/notifications");

  breakfastHour = (char * ) malloc(255);
  strcpy(breakfastHour, path);
  strcat(breakfastHour, "/breakfastHour");

  breakfastMinute = (char * ) malloc(255);
  strcpy(breakfastMinute, path);
  strcat(breakfastMinute, "/breakfastMinute");

  lunchHour = (char * ) malloc(255);
  strcpy(lunchHour, path);
  strcat(lunchHour, "/lunchHour");

  lunchMinute = (char * ) malloc(255);
  strcpy(lunchMinute, path);
  strcat(lunchMinute, "/lunchMinute");

  dinnerHour = (char * ) malloc(255);
  strcpy(dinnerHour, path);
  strcat(dinnerHour, "/dinnerHour");

  dinnerMinute = (char * ) malloc(255);
  strcpy(dinnerMinute, path);
  strcat(dinnerMinute, "/dinnerMinute");

  servingBreakfast = (char * ) malloc(255);
  strcpy(servingBreakfast, path);
  strcat(servingBreakfast, "/servingBreakfast");

  servingLunch = (char * ) malloc(255);
  strcpy(servingLunch, path);
  strcat(servingLunch, "/servingLunch");

  servingDinner = (char * ) malloc(255);
  strcpy(servingDinner, path);
  strcat(servingDinner, "/servingDinner");


  fountainAllDay = (char * ) malloc(255);
  strcpy(fountainAllDay, path);
  strcat(fountainAllDay, "/fountainAllDay");
}

void verifyTimeForMeal() {

  timeClient.update();
  //set hour
  Firebase.setInt("hour", timeClient.getHours());
  //set minutes
  Firebase.setInt("minutes", timeClient.getMinutes());

  int servingB, servingL, servingD;

  servingB = Firebase.getInt(servingBreakfast);

  servingL = Firebase.getInt(servingLunch);

  servingD = Firebase.getInt(servingDinner);

  int brH, brM, lcH, lcM, dnH, dnM;

  brH = Firebase.getInt(breakfastHour);
  brM = Firebase.getInt(breakfastMinute);

  lcH = Firebase.getInt(lunchHour);
  lcM = Firebase.getInt(lunchMinute);

  dnH = Firebase.getInt(dinnerHour);
  dnM = Firebase.getInt(dinnerMinute);

  int currentH, currentM;

  currentH = timeClient.getHours();
  currentM = timeClient.getMinutes();

  if (currentH == brH && currentM == brM && sent == false)
  {
    sent = true;
    BREAKFAST = String( "M" + String(servingB) + "OM");
    softSerial.print(BREAKFAST);
    Serial.println("S-a tr micudejoon");
    delay(200);
  }

  if (currentH == lcH && currentM == lcM && sent == false)
  {
    sent = true;
    LUNCH = String( "L" + String(servingL) + "OL");
    softSerial.print(LUNCH);
    Serial.println("S-a tr pranzu + posibil apa");
    delay(200);
  }

  if (currentH == dnH && currentM == dnM && sent == false)
  {
    sent = true;
    DINNER = String( "M" + String(servingD) + "OM");
    softSerial.print(DINNER);
    Serial.println("S-a tr cina");
    delay(200);
  }

  if (((currentH == brH && currentM == brM + 1) || (currentH == lcH && currentM == lcM + 1) || (currentH == dnH && currentM == dnM + 1)) && sent == true)
  {
    Serial.println("Face sent = false");
    sent = false;
  }

}

void getFountainStatus()
{
  status = Firebase.getInt(fountainAllDay);

  if (status != previousStatus)
  {
    previousStatus = status;
    FOUNTAIN = String( "W" + String(status) + "OW");
    softSerial.print(FOUNTAIN);
    Serial.println("S-a tr apa");
    delay(200);
  }

}

void setup() {

  softSerial.begin(9600);
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
  getFirebasePaths();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  timeClient.begin();

}

void loop() {

  currentTime = timeClient.getEpochTime();

  if (softSerial.available())
  {
    notification = softSerial.readString();

    boolean x = currentTime >= lastTimeOfNotificationFood + 1800;
    Serial.println(x);

    boolean y = currentTime >= lastTimeOfNotificationWater + 1800;
    Serial.println(x);

    if (currentTime >= lastTimeOfNotificationFood + 1800)
    {

      lastTimeOfNotificationFood = currentTime;

      notification.trim();
      Serial.println(notification);

      if (notification.indexOf("DISTANCE") >= 0)
      {
        notificationMessage = "DISTANCE";
        distancePath = String( pathSendTo + String("/") + notificationMessage );
        Firebase.setString(distancePath, "1");
        Serial.println("S-a trimis notificare rezervor mancare");

        if (Firebase.failed()) {
          Serial.print("Sending distance notification failed");
          Serial.println(Firebase.error());
        }
      }
    }

    if (currentTime >= lastTimeOfNotificationWater + 1800)
    {

      lastTimeOfNotificationWater = currentTime;

      notification.trim();
      Serial.println(notification);
      
      if (notification.indexOf("WATER") >= 0)
      {
        notificationMessage = "WATER";
        distancePath = String( pathSendTo + String("/") + notificationMessage );
        Firebase.setString(distancePath, "1");
        Serial.println("S-a trimis notificare apa");

        if (Firebase.failed()) {
          Serial.print("Sending water notification failed");
          Serial.println(Firebase.error());
        }
      }
    }


  }

  verifyTimeForMeal();

  getFountainStatus();

}
