#include <DualVNH5019MotorShield.h>
#include <SharpIR.h>
#include <PinChangeInt.h>

DualVNH5019MotorShield md;

/*
  #define FR A0 //front right sensor
  #define FC A4 //front center sensor
  #define FL A1 //front left sensor
  #define LL A3 //left long range sensor
  #define LS A5 //left short range sensor
  #define R A2 //right sensor
*/
#define FL A1 //front left
#define FC A4 //front center
#define FR A0 //front right
#define RF A2 //right front
#define RB A5//right back
#define LL A3 //left long

#define motor_R_enconder 11
#define motor_L_enconder 3

#define SRmodel 1080
#define LRmodel 20150

#define RECORDS

#define NUM_SENSOR_READINGS_CALI 49

const int maxCalibrationTrial = 30;
boolean calibration = true;

int caliReading[49]; //sensor readings for calibration


double Distance[29];
String gridsToRpi;
volatile int left_encoder_val = 0, right_encoder_val = 0, left_encoder_val2 = 0;

float prevError = 0.0;
float integral = 0.0;

float testSpeed = 200; //180                              //
//float PID_KP =  0.545; // lower = right, higher = left     // computePID() //-1.3 //software project lab
float PID_KP = 0.51; //lounge
//float PID_KP = 5.6; // for own arena use with only arduino loaded
//float PID_KI = 0.01;//0.4;  //0.01        //software project lab                               // computePID()
float PID_KI = 0.022;
//float PID_KI = 0.21; //for own arena use with only arduino loaded
//float PID_KD = 0.005;  //0.05  // computePID()
float PID_KD = 0;
float GRID_DISTANCE[6] = {10.5, 20, 30, 40, 50, 120};   // moveByGrids(int)
//float DG_GRID_DISTANCE[5] = {14.7, 20.5, 29.7, 40.1, 51}; // moveByDgGrids(int)

float DG_GRID_DISTANCE[5] = {10, 20, 30, 40, 50};
float LEFT_ROTATION_DEGREE_OFFSET = 2.05;//2.4;                   // rotateLeft(int)
float RIGHT_ROTATION_DEGREE_OFFSET = 3.6;//3.48;                  // rotateRight(int)
int NUM_OF_SENSOR_READINGS_TO_TAKE = 15;                  // getDistance()
int NUM_OF_SENSOR_READINGS_TO_TAKE_CALIBRATION = 5;       // calAngle() and calDistance()
int COMMAND_DELAY = 60;


const float WHEELCCF = 2 * PI * 3;

// Below Values for HWLab3 Use
float SENSOR_L_RANGE[4] = {41, 31.0, 20, 12};
float SENSOR_R_RANGE[4] = {41, 31.0, 20, 12};
float SENSOR_FL_RANGE[4] = {35, 28.5, 17.5, 10.5};
float SENSOR_FC_RANGE[4] = {35, 28.5, 17.5, 10.5};
float SENSOR_FR_RANGE[4] = {35, 28.5, 17.5, 10.5};
float SENSOR_LR_RANGE[7] = {85, 75, 66, 55, 43, 26, 18};

SharpIR fl =  SharpIR(FL, SRmodel);
SharpIR rf =  SharpIR(RF, SRmodel);
SharpIR ll =  SharpIR(LL, LRmodel);
SharpIR fc =  SharpIR(FC, SRmodel);
SharpIR rb =  SharpIR(RB, SRmodel);
SharpIR fr =  SharpIR(FR, SRmodel);
/*
  float fl[9], fc[9], fr[9], ll[9],ls[9],r[9];
  int flR, fcR, frR, llR,lsR,rR;
  int sensorReadings[6];
  float distance[6];
  char cmd = "";
  String sensorReadingToRpi;
*/


void setup()
{
  Serial.begin(115200);
  //Serial.begin(9600);
  Serial.println("From Arduino: Communicating with Rpi");

  pinMode(motor_R_enconder, INPUT);
  pinMode(motor_L_enconder, INPUT);

  md.init();
  md.setSpeeds(0, 0);

  PCintPort::attachInterrupt(motor_R_enconder, lTick, RISING);
  PCintPort::attachInterrupt(motor_L_enconder, rTick, RISING);

  delay(2000);
}

void loop()
{

  
  if (Serial.available() > 0)
  {
    commandExecution(char(Serial.read()));
  }
  
  /*
    caliAngleFront();
    while(1){
      delay(1000);
    }
    
  */
  /*
  float reading;
   for (int i = 0; i < 29; i++)
      {
        Distance[i] = fc.distance();
      }
      insertionsort(Distance, 29);

      reading =  Distance[29 / 2];
    Serial.println(reading);
   delay(100);
   */
}

void commandExecution(char cmd)
{
  switch (cmd) {
    case 'F':
      //md.setSpeeds(100,100);
      moveByGrids(1);
      delay(100);
      sendSensorReading();
      //Serial.println("k");
      break;
    case 'L':
      //md.setSpeeds(100, -100);
      rotateLeft(90);
      delay(100);
      sendSensorReading();
      //Serial.println("L ok");
      break;
    case 'R':
      //md.setSpeeds(-100, 100);
      rotateRight(90);
      delay(100);
      sendSensorReading();
      //Serial.println("R ok");
      break;

    case 'S':
      sendSensorReading();
      //Serial.println("S ok");
      break;


    case 'C':
      rotateLeft(90);
      rotateLeft(90);
      delay(100);
      caliAngleRight();
      delay(100);
      rotateLeft(90);
      rotateLeft(90);
      Serial.println("k");
      break;
    case 'X':
      caliDistanceFront();
      delay(100);
      Serial.println("k");
      break;
    case 'Y':
      caliAngleRight();
      delay(100);
        Serial.println("k");
      break;

  }
}


//================================================Send distance to RPI========================================================================

void sendSensorReading()
{
  gridsToRpi = "SDATA;1_" + calculateGrids(FL) + ";2_" + calculateGrids(FC) + ";3_" + calculateGrids(FR) + ";4_" + calculateGrids(RF) + ";5_" + calculateGrids(RB) + ";6_" + calculateGrids(LL);
  Serial.println(gridsToRpi);
}
String calculateGrids(int sensor)
{
  double reading;
  double sum = 0;
  switch (sensor) {
    case FL:

      for (int i = 0; i < 29; i++)
      {
        Distance[i] = fl.distance();
      }
      insertionsort(Distance, 29);


      reading = Distance[29 / 2];
      //reading = fl.distance();
      if (reading >= 7 && reading <= 17)
        return "1";
      else if (reading >= 18 && reading <= 29)
        return "2";
      else return "-1";
    case FC:
      for (int i = 0; i < 29; i++)
      {
        Distance[i] = fc.distance();
      }
      insertionsort(Distance, 29);

      reading =  Distance[29 / 2];
      //reading = fc.distance();
      if (reading >= 7 && reading <= 17)
        return "1";
      else if (reading >= 18 && reading <= 21)
        return "2";
      else return "-1";
    case FR:
      for (int i = 0; i < 29; i++)
      {
        Distance[i] = fr.distance();
      }
      insertionsort(Distance, 29);

      reading =  Distance[29 / 2];
      //reading = fr.distance();
      if (reading >= 7 && reading <= 17)
        return "1";
      else if (reading >= 18 && reading <= 28)
        return "2";
      else return "-1";
    case LL:

      for (int i = 0; i < 29; i++)
      {
        Distance[i] = ll.distance();
      }
      insertionsort(Distance, 29);

      reading =  Distance[29 / 2];
      //reading = ll.distance();
      if (reading >= 18 && reading <= 24)
        return "1";
      else if (reading >= 25 && reading <= 32)
        return "2";
      else if (reading >= 33 && reading <= 42 )
        return "3";
      else return "-1";
    case RF:
      for (int i = 0; i < 9; i++)
      {
        Distance[i] = rf.distance();
      }
      insertionsort(Distance, 9);

      reading =  Distance[9 / 2];
      //reading = rf.distance();
      if (reading >= 7  && reading <= 16)
        return "1";
      else if (reading >= 17 && reading <= 27)
        return "2";
      else return "-1";
    case RB:
      for (int i = 0; i < 9; i++)
      {
        Distance[i] = rb.distance();
      }
      insertionsort(Distance, 9);

      reading =  Distance[9 / 2];
      //reading = rb.distance();
      if (reading >= 7 && reading <= 15)
        return "1";
      else if (reading >= 16 && reading <= 28)
        return "2";
      else return "-1";
  }
}

void insertionsort(double array[], int length)
{
  int i, j;
  double temp;
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



//========================================================Calibration==========================================================
void caliAngleFront()
{
  float flDistance, frDistance, error;
  int counter = 0;
  calibration = true;
  flDistance = sensorCaliDistance(FL);
  frDistance = sensorCaliDistance(FR);
  error = flDistance - frDistance;
 

  while (abs(error) > 0.2 && counter < 30)
  {
    //Serial.println(error);
    if (error > 0)
    {
      md.setSpeeds(-100, 100);
      delay(abs(error * 60));
      md.setBrakes(300, 300);
    }
    else
    {
      md.setSpeeds(100, -100);
      delay(abs(error * 50));
      md.setBrakes(300, 300);
    }
    delay(10);
    flDistance = sensorCaliDistance(FL);
    frDistance = sensorCaliDistance(FR);
    error = flDistance - frDistance;
    //Serial.println(error);
    counter++;

  }
  if (counter == 30)
    calibration = false;
}

void caliDistanceFront()
{
  float targetDistance = 5;
  float actualDistance = sensorCaliDistance(FR);
  float difference;
  int counter = 0;
  difference = targetDistance - actualDistance;
  while (abs(difference) > 0.3 && counter < 30)
  {
    if (difference > 0)
    {
      moveBackwardCali(); //move forward for 0.2cm
    }
    else
    {
      moveForwardCali(); //move backward for 0.2cm
    }
    actualDistance = sensorCaliDistance(FR);
    difference = targetDistance - actualDistance;
    counter ++;
  }
  
}

void caliAngleRight()
{
  float rfDistance, rbDistance;
  float error;
  int counter = 0;
  calibration = true;
  rfDistance = sensorCaliDistance(RF);
  rbDistance = sensorCaliDistance(RB);
  error = rfDistance - rbDistance;

  while (abs(error) > 0.2 && counter < 30)
  {
    if (error > 0)
    {
      md.setSpeeds(-100, 100);
      delay(abs(error * 50));
      md.setBrakes(300, 300);
    }
    else
    {
      md.setSpeeds(100, -100);
      delay(abs(error * 50));
      md.setBrakes(300, 300);
    }
    delay(10);
    rfDistance = sensorCaliDistance(RF);
    rbDistance = sensorCaliDistance(RB);
    error = rfDistance - rbDistance;
    counter ++;
  }
  if (counter == 30)
    calibration = false;
}

float sensorCaliDistance(int sensor)
{
  float Distance, voltage;
  switch (sensor)
  {
    case FL:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(FL);
      }
      insertionSort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = (float) caliReading[49 / 2] / 1023 * 5;

      Distance = -2.3286 * pow(voltage, 3) + 17.704 * voltage * voltage + -49.304 * voltage + 53.1;
      //distance =
      return Distance;

    case FR:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(FR);
      }
      insertionSort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = (float) caliReading[49 / 2] / 1023 * 5;
      Distance = -2.8449 * pow(voltage, 3) + 21.106 * voltage * voltage + -56.485 * voltage + 57.856;
      //distance =
      return Distance;
    case RF:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(RF);
      }
      insertionSort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = (float) caliReading[49 / 2] / 1023 * 5;
      Distance = -3.4065 * pow(voltage, 3) + 24.989 * voltage * voltage + -65.787 * voltage + 66.209;
      //distance =
      return Distance;
    case RB:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(RB);
      }
      insertionSort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = (float) caliReading[49 / 2] / 1023 * 5;
      Distance = -1.9274 * pow(voltage, 3) + 14.895 * voltage * voltage + -42.711 * voltage + 49.378; //doenst works when distance less than 2.5cm
      //distance =
      return Distance;

  }

}

void insertionSort(int array[], int length)//sort integers
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

//======================================================Move forward or backward using PID===============================================================================
void lTick()  {
  left_encoder_val++;
  left_encoder_val2++;
}

void rTick()  {
  right_encoder_val++;
}

void resetCounter() {
  left_encoder_val = 0;
  right_encoder_val = 0;
  left_encoder_val2 = 0;
}

void resetPID() {
  prevError = 0;
  integral = 0;
  resetCounter();
}

float pidControlForward(int left_encoder_val, int right_encoder_val) {
  //P = High correction to value, D = Add resistance to P, I = Correct error value to original value
  float error = left_encoder_val - right_encoder_val;
  float derivative, output;

  integral += error;
  derivative = error - prevError;
  output = PID_KP * error + PID_KI * integral + PID_KD * derivative;
  prevError = error;

  return output;
}

void moveByGrids(int grids) {
  moveForward(testSpeed, GRID_DISTANCE[grids - 1]);
  //Serial.println("finish moving");
}

void moveByDgGrids(int grids) {
  moveForward(testSpeed, DG_GRID_DISTANCE[grids - 1]);
}

void moveForward(int sped, float distance)  {
  float output;
  float distanceInTicks;

  distanceInTicks = (distance / WHEELCCF) * 526.25;
  md.setSpeeds(-sped, -sped);
  resetPID();
  while (left_encoder_val2 < distanceInTicks)  {
    output = pidControlForward(left_encoder_val, right_encoder_val);
    delay(1);
    md.setSpeeds(-(sped), -(sped - output));
    //  qwd c.println("output");
    //Serial.println(output);
    //Serial.println("left_encoder_val2, distanceInTicks");
    //Serial.println(left_encoder_val2);
    //Serial.println(distanceInTicks);
  }
  //Serial.println("End moving");
  md.setBrakes(375, 375);
  //  if(!FP_FLAG)
  //      delay(50);
  resetPID();
}

void moveForwardCali()
{
  moveForward(testSpeed, 0.1);
}

void moveBackwardCali()
{
  moveForward(-testSpeed, 0.1);
}


void Brake() {
  md.setBrakes(400, 400);
}
// ================ ROTATION =====================
void rotateLeft(int degree) {
  float output;
  float dis = (degree + LEFT_ROTATION_DEGREE_OFFSET) / 90.0;
  int left_speed = 280;
  int right_speed = 280;
  float actual_distance = (dis * 405);

  delay(1);
  md.setSpeeds(left_speed, -right_speed);
  resetPID();
  while (left_encoder_val2 < actual_distance) {
    output = pidControlForward(left_encoder_val, right_encoder_val);
    delay(1);
    //Serial.println(output);
    md.setSpeeds(left_speed, -right_speed + output);
    if (left_encoder_val2 > actual_distance)
    {
      break;
    }
  }
  md.setBrakes(400, 400);
  resetPID();
  //delay(75);
}

void rotateRight(int degree) {
  float output;
  float dis = (degree + RIGHT_ROTATION_DEGREE_OFFSET) / 90.0;
  int left_speed = 280;
  int right_speed = 280 ;
  float actual_distance = (dis * 405);

  delay(1);
  md.setSpeeds(-left_speed, right_speed);
  resetPID();
  while (left_encoder_val2 < actual_distance) {
    output = pidControlForward(left_encoder_val, right_encoder_val);
    delay(1);

    md.setSpeeds(-left_speed, right_speed - output);
  }
  md.setBrakes(400, 400);
  resetPID();

}
