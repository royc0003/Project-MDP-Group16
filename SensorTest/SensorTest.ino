#include <math.h>
#define RECORDS 49 //number of records for each distance

double A0Reading[RECORDS], A1Reading[RECORDS], A2Reading[RECORDS], A3Reading[RECORDS], A4Reading[RECORDS]; 
void setup() {
  Serial.begin(9600);
  ADCSRA &= ~(bit(ADPS0) | bit(ADPS1) | bit(ADPS2)); // clear prescaler bits
  ADCSRA |= bit(ADPS0) | bit(ADPS1) | bit(ADPS2);   // 128

}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 5; i<45; i++)
  {
    Serial.println("Measuring voltage for" +String(i*2) + "cm for short range sensors.");
    getVoltageRight(i*2);
    Serial.println("Please adjust block to be" + String((i+1)*2) + "cm away.");
    delay(10000);
  }
}

void getVoltageFrontLeft(int distance)
{
  Serial.println("Checking voltage for short range front sensors.");

  for (int i = 0; i < RECORDS; i++)
  {
    delay(200);
    A1Reading[i] = analogRead(A1);
  }
  sort(A1Reading, RECORDS);
  Serial.println("The volatage of front left sensor is: " + String(A1Reading[RECORDS/2]));
}

void getVoltageFrontRight(int distance)
{
  Serial.println("Checking voltage for short range front sensors.");

  for (int i = 0; i < RECORDS; i++)
  {
    delay(200);
    A0Reading[i] = analogRead(A0);
  }
  sort(A0Reading, RECORDS);

  Serial.println("The volatage of front right sensor is: " + String(A0Reading[RECORDS/2]));
}
void getVoltageFrontCenter(int distance)
{
  Serial.println("Checking voltage for front center sensors.");

  for (int i = 0; i < RECORDS; i++)
  {
    delay(200);
    A4Reading[i] = analogRead(A4);
  }
  sort(A4Reading, RECORDS);

  Serial.println("The volatage of front center sensor is: " + String(A4Reading[RECORDS/2]));
}

void getVoltageLeft(int distance)
{
  Serial.println("Checking voltage for left sensor.");

  for (int i = 0; i < RECORDS; i++)
  {
    delay(200);
    A3Reading[i] = analogRead(A3);
  }
 
  sort(A3Reading, RECORDS);
  
  Serial.println("The volatage of left sensor is: " + String(A3Reading[RECORDS/2]));

}
void getVoltageRight(int distance)
{
  Serial.println("Checking voltage for right sensor.");

  for (int i = 0; i < RECORDS; i++)
  {
    delay(200);
    A2Reading[i] = analogRead(A2);
  }
 
  sort(A2Reading, RECORDS);
  
  Serial.println("The volatage of Right sensor is: " + String(A2Reading[RECORDS/2]));

}

void sort(double a[], int len)
{
  double temp;
  for(int i = 0; i < len -1; i++)
  {
    for(int j = 0; j < len-i-1; j++)
    {
      if (a[j] > a[j+1])
      {
        temp = a[j];
        a[j] = a[j+1];
        a[j+1] = temp;
      }
    }
  }
}
