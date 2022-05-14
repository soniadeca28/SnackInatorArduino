#define echopin 8
#define trigpin 9
float distance;
void setup() {
  pinMode(echopin,INPUT);
  pinMode(trigpin,OUTPUT);
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(2);
  digitalWrite(trigpin,LOW);
  distance=pulseIn(echopin,HIGH);
  distance=distance/58;
  Serial.begin(9600);
}

void loop() {
   
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(2);
  digitalWrite(trigpin,LOW);
  distance=pulseIn(echopin,HIGH);
  distance=distance/58; //because the pulse travels at 58 microseconds/cm, and distance is measured in microseconds
  Serial.println(distance);
  delay(2000);
  
}
