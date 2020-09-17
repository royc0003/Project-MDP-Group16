#include <DualVNH5019MotorShield.h>

DualVNH5019MotorShield md;

#define FR A0 //front right sensor
#define FC A4 //front center sensor
#define FL A1 //front left sensor
#define LL A3 //left long range sensor
#define LS A5 //left short range sensor
#define R A2 //right sensor

#define RECORDS 49
float fl[9], fc[9], fr[9], ll[9],ls[9],r[9];
int flR, fcR, frR, llR,lsR,rR;
int sensorReadings[6];
float distance[6];
char cmd = "";
String sensorReadingToRpi;

void setup()
{
  Serial.begin(115200);
  Serial.println("From Arduino: Communicating with Rpi");
  md.init();
  md.setSpeeds(0,0);
  delay(1000);
}

void loop()
{
  if (Serial.available() > 0)
  {
    commandExecution(char(Serial.read()));
  }

}

void commandExecution(char cmd)
{
  switch (cmd) {
    case 'F':
      //md.setSpeeds(100,100);
      moveForward();
      sendSensorReading();
      //Serial.println("F ok");
      break;
    case 'B':
      //md.setSpeeds(-100, -100);
      moveBackward();
      sendSensorReading();
      //Serial.println("B ok");
      break;
    case 'L':
      //md.setSpeeds(100, -100);
      turnLeft();
      sendSensorReading();
      //Serial.println("L ok");
      break;
    case 'R':
      //md.setSpeeds(-100, 100);
      turnRight();
      sendSensorReading();
      //Serial.println("R ok");
      break;
    
    case 'S':
      sendSensorReading();
      //Serial.println("S ok");
      break;
      
    case 'C':
      Serial.println("Calibrating");
      break;
  }
}

void moveForward()
{
  md.setSpeeds(-150, -150); //plus is move backward, left, right
  delay(5000);
  md.setSpeeds(0,0);
}

void moveBackward()
{
  md.setSpeeds(150,150);
  delay(5000);
  md.setSpeeds(0,0);
}

void turnLeft()
{
  md.setSpeeds(200,-200);
  delay(2000);
  md.setSpeeds(0,0);
}

void turnRight()
{
  md.setSpeeds(-200, 200);
  delay(2000);
  md.setSpeeds(0,0);
}

void sendSensorReading()
{
  for (int i=0; i<9; i++)
  {
    fl[i] = calDistance(getSensorReading(RECORDS,FL), FL);
    fc[i] = calDistance(getSensorReading(RECORDS, FC), FC);
    fr[i] = calDistance(getSensorReading(RECORDS, FR), FR);
    ll[i] = calDistance(getSensorReading(RECORDS, LL),LL);
    ls[i] = calDistance(getSensorReading(RECORDS, LS),LS);
    r[i] = calDistance(getSensorReading(RECORDS, R),R);
  }
  
  insertionsortFloat(fl, 9);
  insertionsortFloat(fc, 9);
  insertionsortFloat(fr, 9);
  insertionsortFloat(ll, 9);
  insertionsortFloat(ls, 9);
  insertionsortFloat(r, 9);

  flR = int((fl[9/2])/10);
  fcR = int((fc[9/2])/10);
  frR = int((fr[9/2])/10);
  llR = int((ll[9/2])/10);
  lsR = int((ls[9/2])/10);
  rR = int((ls[9/2])/10);

  sensorReadingToRpi = "SDATA;1_"+  String(flR) + ";2_" + String(fcR) + ";3_" + String(frR) + ";4_" + String(llR) + ";5_" + String(lsR) + ";6_" + String(rR);
  Serial.println(sensorReadingToRpi);
  
}
float calDistance(int reading, int sensor)
{
  float distance;//in cm
  switch (sensor) {
    case FL:
      //distance = -24.04 + 1811796 / (1+pow(reading/0.0000015, 0.57));
      //distance = 18130*pow(reading, -1.319);
      //distance = 25523*pow(reading, -1.382);//10cm to 50cm
      distance = 9439.8*pow(reading,-1.182);
      break;
    case FC:
      //distance = -3.24 + 88316817/(1+pow(reading/0.000021, 0.93));
      //distance = 8610.4*pow(reading, -1.094);
      distance = 16721*pow(reading, -1.232); //30cm to 50cm
      break;
    case FR:
      //distance = -5.14 + 44621115/(1+pow(reading/0.0021,1.25));
      //distance = 142417*pow(reading, -1.687);
      //distance = 169764*pow(reading, -1.705); //from 10cm to 50cm
      distance = 52689*pow(reading, -1.475);//30cm to 50cm
      break;
    case LL:
      //distance = -53.16 +  2482660/(1+pow(reading/pow(7.14, -7), 0.51));
      //distance = 18552*pow(reading, -1.081);
      //distance = 146.13*exp(-0.004*reading);//10cm to 70cm
      distance = 14068*pow(reading, -1.025); //50cm to 70cm
      break;
    case LS:
      //distance = 6.75 + 91.65/(1+pow(reading/103.05, 2.32));
      //distance = 8477.6*pow(reading, -1.11);
      distance = 16713*pow(reading, -1.243);
      break;
    case R:
      //distance = 5.72 + 1618663294/(1+pow(reading/0.018, 1.72));
      //distance = 20699*pow(reading, -1.283);
      distance = 83914*pow(reading, -1.553);
      break;
    default:
      return -1;
  }
  return distance;
}

int getSensorReading(int n, int sensor)
{
  int x[n];
  int i;
  for (i = 0; i < n; i++)
  {
    delay(1);
    x[i] = analogRead(sensor);
  }
  insertionsort(x, n);
  return x[n / 2];
}



void insertionsort(int array[], int length)
{
  int i, j;
  int temp;
  for (i = 1; i < length; i++) {
    for (j = i; j > 0; j--) {
      if (array[j] < array[j - 1]) {
        temp = array[j];
        array[j] = array[j - 1];
        array[j - 1] = temp;
      }
      else
        break;
    }
  }
}

void insertionsortFloat(float array[], int length)
{
  int i, j;
  int temp;
  for (i = 1; i < length; i++) {
    for (j = i; j > 0; j--) {
      if (array[j] < array[j - 1]) {
        temp = array[j];
        array[j] = array[j - 1];
        array[j - 1] = temp;
      }
      else
        break;
    }
  }
}
