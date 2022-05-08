//motor: 
#define dirPin 2 //pin directie motor
#define stepPin 3 //pin pas motor
#define stepsPerRevolution 200 //o rotatie intreaga

//pompa:
#define pumpController 13 //pin pompa

//enable:
#define enableDisableMotor 12 //pin enable

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(pumpController,OUTPUT);

  pinMode(enableDisableMotor,OUTPUT);
}

void loop() {
  
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
  
  //fara sa disable-ui motorul cand merge pompa, din cauza power supply-ului comun, merge si motorul aiurea cand merge pompa
  digitalWrite(enableDisableMotor,HIGH); //disable motor --> nu e negat deci e normally enabled 

  digitalWrite(pumpController,HIGH);
  delay(1000);
  digitalWrite(pumpController,LOW);
  delay(3000);

  digitalWrite(enableDisableMotor,LOW); //enable motor la loc
  
}
