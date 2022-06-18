#include <SoftwareSerial.h>
#include "HX711.h"

#define data A2
#define clk  A1

HX711 scale;
float calibration = 400;
float units;

#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 200
#define disableMotor 12

int maxDistance = 21;

#define pumpController 13

int minWaterLvl = 150;

SoftwareSerial softSerial(5, 6);

unsigned long currentTime, lastTimeFood, lastTimeWater;

#define echopin 8
#define trigpin 9

float distance;

String DISTANCE;
String WATER;
String NOTSERVED;

#define waterSignal A0
#define waterPower 7

int waterLevel;

float serving;
String serv;
char s[] = "";
String received;

int fountainStatus = 1;

int motorMicros;
float multiplySteps;
int errorM;

int case1Distance;
int case2Distance;

void serveFood(float srv, bool serveW)
{
  if (distance < maxDistance)
  {
    scale.set_scale(calibration);
    scale.tare(50);
    controlMotor(srv);

    Serial.print("Served: ");
    Serial.print(units);
    Serial.println(" g");
  }
  else
  {
    Serial.println(distance);
  }

  if (serveW == true && fountainStatus == 0 && waterLevel > minWaterLvl)
  {
    Serial.println("Water was also served");
    controlFountain();
  }
}

void serveWater(int fountainStatus)
{
  if (waterLevel <= minWaterLvl)
  {
    digitalWrite(pumpController, 0);
    return;
  }
  digitalWrite(pumpController, fountainStatus);
}

void verifyDistance()
{
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(2);
  digitalWrite(trigpin, LOW);
  distance = pulseIn(echopin, HIGH);
  distance = distance / 58;
}

void verifyWaterLevel()
{
  digitalWrite(waterPower, HIGH);
  delay(100);
  waterLevel = analogRead(waterSignal);
  digitalWrite(waterPower, LOW);
}
void checkForNotifications()
{
  currentTime = millis();

  verifyDistance();

  verifyWaterLevel();

  if (currentTime >= lastTimeFood && distance >= maxDistance - 1)
  {
    lastTimeFood = currentTime + 1800000;

    DISTANCE = String("DISTANCE");
    softSerial.print(DISTANCE);
    Serial.println("Empty food tank notification sent!");
  }

  if (currentTime >= lastTimeWater && waterLevel <= minWaterLvl + 50)
  {
    lastTimeWater = currentTime + 1800000;

    WATER = String("WATER");
    softSerial.print(WATER);
    Serial.println("Empty fountain notification sent!");
  }

}

void getGrams()
{
  units = scale.get_units();
  if (units < 0)
  {
    units = 0.00;
  }
}

void controlMotor(float srv)
{
  digitalWrite(pumpController, LOW);

  getGrams();
  verifyDistance();

  delay(5000);

  Serial.print("Weight: ");
  Serial.print(units);
  Serial.println(" grams");

  if (srv > 35)
  {
    if (distance < case1Distance)
    {
      errorM = 5;
    }
    else if (distance >= case1Distance && distance < case2Distance)
    {
      errorM = 20;
    }
    else
    {
      errorM = 10;
    }
  }
  else if (srv > 20 && srv <= 35)
  {
    if (distance < case1Distance)
    {
      errorM = 5;
    }
    else if (distance >= case1Distance && distance < case2Distance)
    {
      errorM = 10;
    }
    else
    {
      errorM = 7;
    }
  }
  else
  {
    errorM = 5;
  }

  case1Distance = 14;
  case2Distance = 17;

  while (units <= srv - errorM  && distance < maxDistance)
  {
    if (distance < case1Distance)
    {
      Serial.println("Case 1");
      Serial.println(units);
      Serial.println(distance);

      motorMicros = 2000;
      multiplySteps = 2;

      digitalWrite(dirPin, HIGH);
      digitalWrite(disableMotor, LOW);

      getGrams();

      if (units < srv - errorM)
      {
        for (int i = 0; i < stepsPerRevolution * multiplySteps; i++) {

          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
      }

      motorMicros = 1500;

      digitalWrite(dirPin, LOW);

      getGrams();

      if (units < srv - errorM)
      {
        for (int i = 0; i < stepsPerRevolution * multiplySteps; i++) {

          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
      }
      digitalWrite(disableMotor, HIGH);
    }
    else if (distance >= case1Distance && distance < case2Distance)
    {
      Serial.println("Case 2");
      Serial.println(units);
      Serial.println(distance);

      motorMicros = 3000;
      multiplySteps = 3;

      digitalWrite(dirPin, HIGH);
      digitalWrite(disableMotor, LOW);

      getGrams();

      if (units < srv - errorM)
      {
        for (int i = 0; i < stepsPerRevolution / multiplySteps; i++) {

          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
      }

      digitalWrite(dirPin, LOW);

      getGrams();

      if (units < srv - errorM)
      {
        for (int i = 0; i < stepsPerRevolution / multiplySteps; i++) {

          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
      }
      digitalWrite(disableMotor, HIGH);
    }
    else if (distance >= case2Distance)
    {
      Serial.println("Case 2");
      Serial.println(units);
      Serial.println(distance);

      motorMicros = 4000;
      multiplySteps = 3;

      digitalWrite(dirPin, HIGH);
      digitalWrite(disableMotor, LOW);

      getGrams();

      if (units < srv - errorM)
      {
        for (int i = 0; i < stepsPerRevolution / multiplySteps; i++) {

          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
      }

      digitalWrite(dirPin, LOW);

      getGrams();

      if (units < srv - errorM)
      {
        for (int i = 0; i < stepsPerRevolution / multiplySteps; i++) {

          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
      }
      digitalWrite(disableMotor, HIGH);
    }
    getGrams();
    verifyDistance();
  }


  digitalWrite(disableMotor, HIGH);
}

void controlFountain()
{
  digitalWrite(pumpController, HIGH);
  delay(4000);
  digitalWrite(pumpController, LOW);
}

void setup() {
  Serial.begin(9600);
  softSerial.begin(9600);

  scale.begin(data, clk);
  scale.set_scale(calibration);
  scale.tare();

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(pumpController, OUTPUT);
  digitalWrite(pumpController, LOW);

  pinMode(disableMotor, OUTPUT);
  digitalWrite(disableMotor, HIGH);

  pinMode(waterPower, OUTPUT);
  digitalWrite(waterPower, LOW);

  pinMode(echopin, INPUT);
  pinMode(trigpin, OUTPUT);

  verifyWaterLevel();
  verifyDistance();
  getGrams();
}

void loop() {

  digitalWrite(disableMotor, HIGH);

  if (softSerial.available())
  {
    received = softSerial.readString();
    Serial.println(received);

    if (received.indexOf("B") >= 0 && received.indexOf("OB") >= 0)
    {
      Serial.println("Received breakfast");
      Serial.println(received);
      received = received.substring(received.indexOf("B") + 1, received.indexOf("OB"));
      received.trim();
      serving = received.toFloat();

      serveFood(serving, true);
    }
    else if (received.indexOf("M") >= 0 && received.indexOf("OM") >= 0)
    {
      Serial.println("Received meal");
      received = received.substring(received.indexOf("M") + 1, received.indexOf("OM"));
      received.trim();
      serving = received.toFloat();

      serveFood(serving, false);
    }
    else if (received.indexOf("W") >= 0 && received.indexOf("OW") >= 0)
    {
      Serial.println("Received water status");
      received = received.substring(received.indexOf("W") + 1, received.indexOf("OW"));
      received.trim();
      fountainStatus = received.toInt();
    }

  }

  checkForNotifications();
  serveWater(fountainStatus);

}
