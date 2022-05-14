#include <SoftwareSerial.h>

SoftwareSerial softSerial(5,6);

unsigned long currentTime, lastTime;

#define echopin 8 // echo pin of the distance sensor
#define trigpin 9 // trig pin of the distance sensor

float distance; // distance calculated by distance sensor

String DISTANCE;

int serving;
String serv;
char s[]="";
String received;

int fountainStatus = 1;


void serveFood(int srv, bool serveW)
{
  Serial.print("Au fost servite ");
  Serial.print(srv);
  Serial.println(" g");

  if(serveW == true && fountainStatus == 0)
  {
    Serial.println("S-a dat si apa~");
  }
}

void serveWater(int fountainStatus)
{
  if(fountainStatus == 1)
  {
    Serial.println("~");//wave form for water!
  }
}

void sendTrigAndCalculateDistance()
{
  currentTime = millis();
  
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(2);
  digitalWrite(trigpin,LOW);
  distance=pulseIn(echopin,HIGH);
  distance=distance/58;

  if(distance > 20 && currentTime >= lastTime)
  {
    lastTime = currentTime + 600000;
    DISTANCE = String("   " + String(distance));
    softSerial.print(DISTANCE);
    Serial.println("S-a trimis notificare ca nu mai e mangiare");
    delay(5000);
  }
}

void setup() {
  Serial.begin(9600);
  softSerial.begin(9600);

  pinMode(echopin,INPUT); //echo pin is input because it stores the duration of ultrasound to the object and back in microseconds
  pinMode(trigpin,OUTPUT); //echo pin is output because it lets the ultrasound be sent

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
  if (softSerial.available()) 
  {

    sendTrigAndCalculateDistance(); // send notification if distance is too big
    
    received = softSerial.readString();

    Serial.print("S-a primit: ");
    Serial.println(received);
    Serial.println();

    if(received.startsWith("   "))
    {
      Serial.println(received);
      received.trim();
      serving = received.toInt();

      serveFood(serving, true);
    }
    else if(received.startsWith("  "))
    {
      received.trim();
      serving = received.toInt();

      serveFood(serving, false);

    }
    else if(received.startsWith(" "))
    {
      received.trim();
      fountainStatus = received.toInt();

      Serial.println("S-a primit statusul apei: ");
      Serial.println(fountainStatus);
    }
    
  }
  
  serveWater(fountainStatus);

}
