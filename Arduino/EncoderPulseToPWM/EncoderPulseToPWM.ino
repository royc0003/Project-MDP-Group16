#include <DualVNH5019MotorShield.h>

#define LEFT 3
#define RIGHT 11
DualVNH5019MotorShield md;
unsigned long periodL, periodR, rpmL, rpmR;
void setup()
{
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  Serial.begin(9600);
  md.init();
  md.setM1Speed(400);
  md.setM2Speed(400); 
}
void loop()
{
   delay(500);
   periodL = 2*pulseIn(LEFT, HIGH);
   periodR = 2*pulseIn(RIGHT, HIGH);
   rpmL = 60/(562.25*periodL*pow(10, -6));
   rpmR = 60/(562.25*periodR*pow(10, -6));
   Serial.println("rpmL");
   Serial.println(rpmL);
   Serial.println("rpmR");
   Serial.println(rpmR);
  
}
