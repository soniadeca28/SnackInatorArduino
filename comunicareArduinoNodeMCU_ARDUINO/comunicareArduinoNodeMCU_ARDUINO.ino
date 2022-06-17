#include <SoftwareSerial.h>
#include "HX711.h"

//cantar
#define data A2
#define clk  A1

HX711 scale;
float calibration = 400; // this calibration factor must be adjusted according to your load cell
float units;

//motor:
#define dirPin 2 //pin directie motor
#define stepPin 3 //pin pas motor
#define stepsPerRevolution 200 //o rotatie intreaga
//enable:
#define disableMotor 12 //pin enable

//pompa:
#define pumpController 13 //pin pompa

SoftwareSerial softSerial(5, 6);

unsigned long currentTime, lastTimeFood, lastTimeWater;

#define echopin 8 // echo pin of the distance sensor
#define trigpin 9 // trig pin of the distance sensor

float distance; // distance calculated by distance sensor

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

int maxDistance;
int case1Distance;
int case2Distance;

void serveFood(float srv, bool serveW)
{
  Serial.println(units);
  Serial.println(distance);
  if (distance < 22)
  {
    scale.set_scale(calibration);
    scale.tare(50);
    controlMotor(srv);
  }
  else
  {
    Serial.println(distance);
  }

  Serial.print("Served: ");
  Serial.print(srv);
  Serial.println(" g");

  if (serveW == true && fountainStatus == 0 && waterLevel > 150)
  {
    Serial.println("Water was also served");
    controlFountain();
  }
}

void serveWater(int fountainStatus)
{
  if (waterLevel <= 150)
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
  //SAVE WATER LEVEL
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

  if (currentTime >= lastTimeFood && distance > 20)
  {
    lastTimeFood = currentTime + 1800000;

    DISTANCE = String("DISTANCE");
    softSerial.print(DISTANCE);
    Serial.println("Empty food tank notification sent!");
    Serial.println(distance);
  }

  if (currentTime >= lastTimeWater && waterLevel <= 200)
  {
    lastTimeWater = currentTime + 1800000;

    WATER = String("WATER");
    softSerial.print(WATER);
    Serial.println("Empty fountain notification sent!");
    Serial.println(waterLevel);
  }

}

void getGrams()
{
  units = scale.get_units(), 5;
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

  if(srv < 30)
  {
    errorM = 10;
  }
  else if (srv >30 && srv < 40)
  {
    errorM = 15;
  }
  else
  {
    errorM = 20;
  }
  maxDistance = 21;
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

      for (int i = 0; i < stepsPerRevolution * multiplySteps; i++) {
        if (units < srv - errorM)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
        else
        {
          break;
        }
      }

      motorMicros = 1500;

      digitalWrite(dirPin, LOW);

      for (int i = 0; i < stepsPerRevolution * multiplySteps; i++) {
        if (units < srv - errorM)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
        else
        {
          break;
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

      for (int i = 0; i < stepsPerRevolution/multiplySteps; i++) {
        if (units < srv - errorM)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
        else
        {
          break;
        }
      }

      digitalWrite(dirPin, LOW);

      for (int i = 0; i < stepsPerRevolution/multiplySteps; i++) {
        if (units < srv - errorM)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
        else
        {
          break;
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

      for (int i = 0; i < stepsPerRevolution/multiplySteps; i++) {
        if (units < srv - errorM)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
        else
        {
          break;
        }
      }

      digitalWrite(dirPin, LOW);

      for (int i = 0; i < stepsPerRevolution/multiplySteps; i++) {
        if (units < srv - errorM)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(motorMicros);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(motorMicros);
        }
        else
        {
          break;
        }
      }
      digitalWrite(disableMotor, HIGH);;
    }
    getGrams();
    verifyDistance();
  }

  //fara sa disable-ui motorul cand merge pompa, din cauza power supply-ului comun, merge si motorul aiurea cand merge pompa
  digitalWrite(disableMotor, HIGH); //disable motor --> nu e negat deci e normally enabled
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

    if (received.indexOf("B") >= 0 && received.indexOf("OB") >= 0)
    {
      Serial.println(received);
      received = received.substring(received.indexOf("B") + 1, received.indexOf("OB"));
      received.trim();
      serving = received.toFloat();

      serveFood(serving, true);
    }
    else if (received.indexOf("M") >= 0 && received.indexOf("OM") >= 0)
    {
      received = received.substring(received.indexOf("M") + 1, received.indexOf("OM"));
      received.trim();
      serving = received.toFloat();

      serveFood(serving, false);
    }
    else if (received.indexOf("W") >= 0 && received.indexOf("OW") >= 0)
    {
      received = received.substring(received.indexOf("W") + 1, received.indexOf("OW"));
      received.trim();
      fountainStatus = received.toInt();
    }

  }

  checkForNotifications(); // send notification if distance is too big
  //send notification if water level is too low
  serveWater(fountainStatus);

}
