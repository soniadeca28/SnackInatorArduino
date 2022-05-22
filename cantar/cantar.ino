#include "HX711.h"
#define DOUT A1
#define CLK  A0
HX711 scale;
float calibration_factor = 400; // this calibration factor must be adjusted according to your load cell
float units;
void setup ()
{
 Serial.begin(9600);
 scale.begin(DOUT, CLK);
 Serial.println("HX711 calibration sketch");
 Serial.println("Remove all weight from scale");
 Serial.println("After readings begin, place known weight on scale");
 Serial.println("Press + or a to increase calibration factor");
 Serial.println("Press - or z to decrease calibration factor");
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
 scale.tare();  //Reset the scale to 0
 long zero_factor = scale.read_average(); //Get a baseline reading
 Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
 Serial.println(zero_factor);
}
void loop()
{
 Serial.print("Reading");
 units = scale.get_units(), 5;
 if (units < 0)
{
   units = 0.00;
}
  Serial.print("Weight: ");
 Serial.print(units);
 Serial.print(" grams"); 
 Serial.print(" calibration_factor: ");
 Serial.print(calibration_factor);
 Serial.println();
 if(Serial.available())
 {
   char temp = Serial.read();
   if(temp == '+' || temp == 'a')
     calibration_factor += 1;
   else if(temp == '-' || temp == 'z')
     calibration_factor -= 1;
 }
 if(Serial.available())
 {
   char temp = Serial.read();
   if(temp == 't' || temp == 'T')
     scale.tare();  //Reset the scale to zero      
 }
 delay(5000);
} 
