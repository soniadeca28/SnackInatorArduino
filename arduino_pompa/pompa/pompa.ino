#include <TimeLib.h>


#define pumpController 13

void setup() {
  pinMode(pumpController,OUTPUT);
}

void loop() {
  digitalWrite(pumpController,LOW);
  delay(1000);
  digitalWrite(pumpController,HIGH);
  delay(3000);
  
}
