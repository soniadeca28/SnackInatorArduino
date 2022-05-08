#include <SoftwareSerial.h>

SoftwareSerial softSerial(D6,D5);

void setup() {
Serial.begin(9600);
softSerial.begin(9600);
}

void loop() {
  
while(softSerial.available())
{
  Serial.println(softSerial.readString());
}
}
