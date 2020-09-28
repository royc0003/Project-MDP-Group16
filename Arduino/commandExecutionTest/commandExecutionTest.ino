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
#define R A2 //right
#define LL A3 //left long
#define FC A4 //front center
#define LS A5 //left short
#define FR A0 //front right

#define motor_R_enconder 11
#define motor_L_enconder 3

#define SRmodel 1080
#define LRmodel 20150

#define RECORDS 39


double distance[RECORDS];
String gridsToRpi;
volatile int left_encoder_val = 0, right_encoder_val = 0, left_encoder_val2 = 0;

float prevError = 0.0;
float integral = 0.0;

float testSpeed = 200; //180                              //
float PID_KP =  0.545; // lower = right, higher = left     // computePID() //-1.3 //software project lab
//float PID_KP = 15; // for own arena use with only arduino loaded
float PID_KI = 0.009;//0.4;  //0.01        //software project lab                               // computePID()
//float PID_KI = 0.87; //for own arena use with only arduino loaded
float PID_KD = 0.005;  //0.05                                       // computePID()
float GRID_DISTANCE[6] = {10.3, 20, 30, 40, 50, 120};   // moveByGrids(int)
//float DG_GRID_DISTANCE[5] = {14.7, 20.5, 29.7, 40.1, 51}; // moveByDgGrids(int)
//float PID_KP = 0;
//float PID_KI = 0;
//float PID_KD = 0;
float DG_GRID_DISTANCE[5] = {10,20,30,40,50};
float LEFT_ROTATION_DEGREE_OFFSET = -1;                   // rotateLeft(int)
float RIGHT_ROTATION_DEGREE_OFFSET = 3.5;                  // rotateRight(int)
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
SharpIR r =  SharpIR(R, SRmodel);
SharpIR ll =  SharpIR(LL, LRmodel);
SharpIR fc =  SharpIR(FC, SRmodel);
SharpIR ls =  SharpIR(LS, SRmodel);
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
  md.setSpeeds(0,0);

  PCintPort::attachInterrupt(motor_R_enconder, lTick, RISING);
  PCintPort::attachInterrupt(motor_L_enconder, rTick, RISING);
  
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
      moveByGrids(2);
      sendSensorReading();
      //Serial.println("k");
      break;
    case 'L':
      //md.setSpeeds(100, -100);
      rotateLeft(90);
      sendSensorReading();
      //Serial.println("L ok");
      break;
    case 'R':
      //md.setSpeeds(-100, 100);
      rotateRight(90);
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




void sendSensorReading()
{
  gridsToRpi = "SDATA;1_"+ calculateGrids(FL) + ";2_" + calculateGrids(FC) + ";3_" + calculateGrids(FR) + ";4_" + calculateGrids(LL) + ";5_" + calculateGrids(LS) + ";6_" + calculateGrids(R);
  Serial.println(gridsToRpi);
}
String calculateGrids(int sensor)
{
  double reading;
  double sum = 0;
  switch (sensor) {
    case FL:
      /*
        for(int i = 0; i<9; i++)
        {
        distance[i] = getDistance(FL);
        }
        insertionsort(distance, 9);

        return distance[9/2];
      */
      reading = fl.distance();
      if (reading > 7 && reading <= 16)
        return "1";
      else if (reading >= 17 && reading <= 28)
        return "2";
      else return "-1";
    case FC:
      reading = fc.distance();
      if (reading > 7 && reading <= 13)
        return "1";
      else if (reading >= 14 && reading <= 24)
        return "2";
      else return "-1";
    case FR:
      reading = fr.distance();
      if (reading > 8 && reading <= 18)
        return "1";
      else if (reading >= 19 && reading <= 29)
        return "2";
      else return "-1";
    case LL:
    
      for(int i = 0; i<RECORDS; i++)
        {
        distance[i] = ll.distance();
        }
        insertionsort(distance, RECORDS);
     
        reading = distance[RECORDS/2];
      if (reading > 21 && reading <= 28)
        return "3";
      else if (reading >= 29 && reading <=39 )
        return "4";
      else return "-1";
    case LS:
      reading = ls.distance();
      if (reading > 7 && reading <= 11)
        return "1";
      else if (reading >= 12 && reading <= 25)
        return "2";
      else return "-1";
     case R:
      reading = r.distance();
      if (reading > 7 && reading <= 12)
        return "1";
      else if (reading >= 13 && reading <= 25)
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
  moveForward(testSpeed, GRID_DISTANCE[grids-1]);
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
    md.setSpeeds(-(sped),-(sped - output));
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
