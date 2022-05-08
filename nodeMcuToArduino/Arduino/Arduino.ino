#include <SoftwareSerial.h>

SoftwareSerial softSerial(5,6);

void setup() {
  Serial.begin(9600);
  softSerial.begin(9600);
  delay(200);
}

void loop() {
softSerial.print("Marian ");
delay(1000);
}
