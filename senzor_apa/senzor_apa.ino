
#define levelSignal A0
#define power 7

int level;

void setup() {
  Serial.begin(9600);
  pinMode(power,OUTPUT);
  digitalWrite(power,LOW);
}

void loop() {
  
  digitalWrite(power,HIGH);
  delay(100);
  level = analogRead(levelSignal);
  digitalWrite(power,LOW);

  Serial.print("Level: ");
  Serial.println(level);

  delay(1000);
}
