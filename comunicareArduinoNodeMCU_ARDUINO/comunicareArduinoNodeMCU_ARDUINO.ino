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
#define enableDisableMotor 12 //pin enable

//pompa:
#define pumpController 13 //pin pompa

SoftwareSerial softSerial(5, 6);

unsigned long currentTime, lastTime;

#define echopin 8 // echo pin of the distance sensor
#define trigpin 9 // trig pin of the distance sensor

float distance; // distance calculated by distance sensor

String DISTANCE;
String WATER;

#define waterSignal A0
#define waterPower 7

int waterLevel;

float serving;
String serv;
char s[] = "";
String received;

int fountainStatus = 1;


void serveFood(float srv, bool serveW)
{
  controlMotor(srv);
  Serial.print("Au fost servite ");
  Serial.print(srv);
  Serial.println(" g");

  if (serveW == true && fountainStatus == 0)
  {
    Serial.println("S-a dat si apa~");
    controlFountain();
  }
}

void serveWater(int fountainStatus)
{
  digitalWrite(pumpController, fountainStatus);
}

void checkForNotifications()
{
  currentTime = millis();

  //SAVE DISTANCE
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(2);
  digitalWrite(trigpin, LOW);
  distance = pulseIn(echopin, HIGH);
  distance = distance / 58;

  //SAVE WATER LEVEL
  digitalWrite(waterPower, HIGH);
  delay(100);
  waterLevel = analogRead(waterSignal);
  digitalWrite(waterPower, LOW);

  if (currentTime >= lastTime)
  {
    //lastTime = currentTime + 600000;
    lastTime = currentTime + 30000;

    Serial.print("Distance: ");
    Serial.println(distance);

    Serial.print("Water: ");
    Serial.println(waterLevel);

    if (distance > 20)
    {
      DISTANCE = String("DISTANCE");
      softSerial.print(DISTANCE);
      Serial.println("S-a trimis notificare ca nu mai e mangiare");
    }
    if (waterLevel <= 300)
    {
      WATER = String("WATER");
      softSerial.print(WATER);
      Serial.println("S-a trimis notificare ca nu mai e apa");
    }
  }
}

void getGrams()
{
  Serial.print("Reading scale first");
  units = scale.get_units(), 5;
  if (units < 0)
  {
    units = 0.00;
  }

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration += 1;
    else if (temp == '-' || temp == 'z')
      calibration -= 1;
  }
  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == 't' || temp == 'T')
      scale.tare();  //Reset the scale to zero
  }

  Serial.print("Weight: ");
  Serial.print(units);
  Serial.println(" grams");
}

void controlMotor(float srv)
{
  digitalWrite(pumpController, LOW);
  
  getGrams();

  delay(5000);

  Serial.print("Weight: ");
  Serial.print(units);
  Serial.println(" grams");

  while (units < srv)
  {

    Serial.print("Serving: "); Serial.println(serving);
    Serial.print("In bowl: "); Serial.println(units);

    digitalWrite(enableDisableMotor, LOW); //enable motor

    // Set the spinning direction clockwise:
    digitalWrite(dirPin, HIGH);

    // Spin the stepper motor 1 revolution slowly:
    for (int i = 0; i < stepsPerRevolution; i++) { //o rotatie
      // These four lines result in 1 step:
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(1000);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(1000);
    }

    digitalWrite(dirPin, LOW);

    for (int i = 0; i < stepsPerRevolution; i++) { //o rotatie
      // These four lines result in 1 step:
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(1000);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(1000);
    }

    getGrams();
  }

  //fara sa disable-ui motorul cand merge pompa, din cauza power supply-ului comun, merge si motorul aiurea cand merge pompa
  digitalWrite(enableDisableMotor, HIGH); //disable motor --> nu e negat deci e normally enabled
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

  //cantar
  scale.begin(data, clk);
  scale.set_scale(calibration); //Adjust to this calibration factor
  scale.tare();  //Reset the scale to 0

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(pumpController, OUTPUT);
  digitalWrite(pumpController, LOW);

  pinMode(enableDisableMotor, OUTPUT);
  digitalWrite(enableDisableMotor, HIGH); //disable motor --> nu e negat deci e normally enabled

  pinMode(waterPower, OUTPUT);
  digitalWrite(waterPower, LOW);

  pinMode(echopin, INPUT); //echo pin is input because it stores the duration of ultrasound to the object and back in microseconds
  pinMode(trigpin, OUTPUT); //echo pin is output because it lets the ultrasound be sent

  //sendTrigAndCalculateDistance(); //calculate first distance

  while (!softSerial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
    Serial.print(".");
  }
  Serial.println("connected!");
}

void loop() {

  // Serial.println(softSerial.readString());
  digitalWrite(enableDisableMotor, HIGH);
  
  if (softSerial.available())
  {
    received = softSerial.readString();

    Serial.print("S-a primit: ");
    Serial.println(received);
    Serial.println();

    if (received.indexOf("L") >= 0 && received.indexOf("OL") >= 0)
    {
      Serial.println(received);
      received = received.substring(received.indexOf("L") + 1, received.indexOf("OL"));
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
      Serial.print("Apa dupa modificari: ");
      Serial.println(received);
      fountainStatus = received.toInt();

      Serial.println("S-a primit statusul apei: ");
      Serial.println(fountainStatus);
    }

  }

  checkForNotifications(); // send notification if distance is too big
  //send notification if water level is too low
  serveWater(fountainStatus);

  getGrams();
  delay(2000);

}
