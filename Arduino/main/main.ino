#include <DualVNH5019MotorShield.h>
#include <SharpIR.h>
#include <PinChangeInt.h>

DualVNH5019MotorShield md;


//================== for sensors =======================
#define FL A1 //front left
#define FC A4 //front center
#define FR A0 //front right
#define RF A2 //right front
#define RB A5//right back
#define LL A3 //left long

#define SRmodel 1080
#define LRmodel 20150

SharpIR fl =  SharpIR(FL, SRmodel);
SharpIR rf =  SharpIR(RF, SRmodel);
SharpIR ll =  SharpIR(LL, LRmodel);
SharpIR fc =  SharpIR(FC, SRmodel);
SharpIR rb =  SharpIR(RB, SRmodel);
SharpIR fr =  SharpIR(FR, SRmodel);

//=================  for calibration ======================
#define NUM_SENSOR_READINGS_CALI 50

#define CALI_DISTANCE_OFFSET_R -0.1//-0.7//0.3 //offset for distance calibration using front sensor
#define CALI_DISTANCE_OFFSET_L -0.2//-0.1//0//0.1//0
#define LR_OFFSET 0.78//0.75//0.8//0.85//0.9//1.2//1.1//0.5//0.9 //offset for angle calibration using FR and FL sensor
#define LC_OFFSET 0.7//1.2 //offset for angle calibration using FL and FC sensor
#define CR_OFFSET -0.4 //offset for angle calibration using FR and FC sensor
#define R_OFFSET 1.9//1.8//1.2////1.4//1.6//1.4//1.1//-0.5 //offset for andle calibration using right sensors
#define RF_OFFSET 0.5

#define ERROR_RIGHT 0
#define ERROR_FRONT 0

const int maxCalibrationTrial = 30;


float caliReading[50]; //sensor readings for calibration


String gridsToRpi = "";


//========================= for movement ====================================
#define motor_R_enconder 11
#define motor_L_enconder 3

volatile int left_encoder_val = 0, right_encoder_val = 0, left_encoder_val2 = 0;

float prevError = 0.0;
float integral = 0.0;


float testSpeed = 320; //180                              //
float PID_KP = 1.6; // lower = right, higher = left     // computePID() //1.6
float PID_KI = 0.2;//0.3;//0.5; //0.06; //0.00222;//0.01                                       // computePID()
float PID_KD = 0; //0.05                                       // computePID()
float GRID_DISTANCE[10] = {10.15, 20.3, 29.7, 40.1, 51, 60, 70, 80, 90, 100};  // moveByGrids(int)
float DG_GRID_DISTANCE[5] = {14.7, 20.5, 29.7, 40.1, 51}; // moveByDgGrids(int)
float LEFT_ROTATION_DEGREE_OFFSET = -6.5;//-4.5                   // rotateLeft(int)
float RIGHT_ROTATION_DEGREE_OFFSET = -8.2;//-8.3;//-8.8;////-7.8//-3.8;   //-2.8               // rotateRight(int)
//int NUM_OF_SENSOR_READINGS_TO_TAKE = 15;                  // getDistance()
int NUM_OF_SENSOR_READINGS_TO_TAKE_CALIBRATION = 5;       // calAngle() and calDistance()
//int COMMAND_DELAY = 60;


const float WHEELCCF = 2 * PI * 3;


//=================================for new PID===================================

int encoder_val_R = 0, encoder_val_L = 0;
int counter_MR = 0, counter_ML = 0;
int currentTime;
unsigned long startTime_MR, endTime_MR, duration_MR;
unsigned long startTime_ML, endTime_ML, duration_ML;
String fastestString, calibrateString;
char header, func;

double input_MR = 0, input_ML = 0;
double pidOutput_MR = 0, pidOutput_ML = 0;
double prev_pidOutput_MR = 0, prev_pidOutput_ML = 0;
double error_MR = 0, error_ML = 0;
double prev_error_MR = 0, prev_error_ML = 0;
double prev_prev_error_MR = 0, prev_prev_error_ML = 0;
double total_Dis = 0;

float kp_MR = 2.2;//0.085;
float ki_MR = 0.12;//0.1;//0.2;//0.1;//0.10;//0.0800;
float kd_MR = 0;//0;//0.160;
//float kp_ML = 0.110;
// NOTE MOTOR_LEFT (Purple Line)
float kp_ML = 1.5;//0.085;//0.085;//0.0100;//0.025;//0.065;//0.075;//0.11;
float ki_ML =0.13;//0.14;//0.12;// 0.11;//0.12;//0.15;//0.1;//0.10;//0.09;//0.065;//0.080;
float kd_ML = 0;//0.160;//0.014;
double setpoint = 130;
double setpoint_RT = 100;

float MIN_DISTANCE_CALIBRATE = 9.5;
float ANGLE_CALIBRATION_THRESHOLD = 0.05;
float LEFT_TO_WALL_DISTANCE_THRESHOLD[2] = {MIN_DISTANCE_CALIBRATE - 0.05, MIN_DISTANCE_CALIBRATE + 0.05};  // calDistance()
float RIGHT_TO_WALL_DISTANCE_THRESHOLD[2] = {MIN_DISTANCE_CALIBRATE - 0.05, MIN_DISTANCE_CALIBRATE + 0.05}; // calDistance()
float FRONT_TO_WALL_DISTANCE_THRESHOLD[2] = {MIN_DISTANCE_CALIBRATE - 0.05, MIN_DISTANCE_CALIBRATE + 0.05}; // calDistanceFront()
const int AVERAGE_FRONT = 0;
const int AVERAGE_READINGS = 1;
const int AVERAGE_DISCRETE = 2;
const int AVERAGE_FRONT_DISCRETE = 3;
int SENSOR_READING_TYPE = AVERAGE_FRONT_DISCRETE;
const int FRONT_LEFT_SENSOR = 1;
const int FRONT_MIDDLE_SENSOR = 2;
const int FRONT_RIGHT_SENSOR = 3;
const int LEFT_SHORT_SENSOR = 4;
const int LEFT_LONG_SENSOR = 5;
const int RIGHT_SHORT_SENSOR = 6;
const int NUM_SAMPLES = 10;
bool initial = true;
const int initial_Speed = 150;
const int rotate_left_Lspeed = 180;///150;//130;//140;
const int rotate_left_Rspeed = 230;//190;//200;//180;//150;//180;//170//160;
int rotate_left_LspeedInitial = rotate_left_Lspeed;
int rotate_left_RspeedInitial = rotate_left_Rspeed;
int rotate_left_LUpdate = 0;
int rotate_left_RUpdate = 0;
int rotate_left_counter = 0;
int front_calibration_counter = 0;



void setup()
{
  Serial.begin(115200);
  //Serial.begin(9600);
  Serial.println("From Arduino: Communicating with Rpi");

  pinMode(motor_R_enconder, INPUT);
  pinMode(motor_L_enconder, INPUT);

  md.init();
  md.setSpeeds(0, 0);

  PCintPort::attachInterrupt(motor_R_enconder, countPulse_MR, RISING);
  PCintPort::attachInterrupt(motor_L_enconder, countPulse_ML, RISING);

  delay(2000);
}

void loop()
{

/*
 * ***CheckList to Follow*****
 * 
 *(1) Fully charge and discharge for 3 mins
 *(2) Check voltage is between 6.28v - 6.30v
 *(3) Did you do a full angle calibration?
 *    - FC, FL , FR
 *    - Top Right, Top Bottom 
 *(4) Did you do the distance calibration?
 *    - Front Right
 *    - Right Center
 *    - Left Center
 *(5) Did you do a Sensor Check?
 *    (Remember to do within arena, after hitting the 'full calibration'
 *(6) PID?
 *    - Move Forward == only 1 grid?
 *    - Rotate Left - 90 Degrees?
 *    - Rotate Right - 90 Degrees?
 *(7) Final Check?
 *    - Full Calibration (is it too far from start point?)


*/
  
  /*
  for(int i = 0; i<5; i++)
  {
    goStraightInGrids(1);
    delay(500);
  }
  */

  
  
//  rotateRight(90);
//      rotateRight(90);
//      delay(200);
//      caliAngleRight();
//      delay(200);
//      caliDistanceFront();
//      delay(100);
//      caliAngleRight();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      caliAngleFrontLR();
//      delay(200);
//      caliDistanceFront();
//      delay(200);
//      caliAngleFrontLR();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//  while(1)
//  {
//    delay(10000);
//  }

  //Full calibration
  /*
      rotateRight(90);
      rotateRight(90);
      delay(100);
      caliAngleRight();
      delay(100);
      caliDistanceFront();
      delay(100);
      caliAngleRight();
      delay(100);
      rotateRight(90);
      delay(100);
      caliAngleFrontLR();
      delay(100);
      caliDistanceFront();
      delay(100);
      caliAngleFrontLR();
      delay(100);
      rotateRight(90);
      delay(100);
      Serial.println("k");
        
  delay(2000);
      rotateRight(90);
      delay(100);
      caliAngleRight();
  delay(2000);
  */
      
      
      //caliAngleRight();
      //delay(10000);
  /*
  delay(2000);
      goStraightInGrids(1);
      delay(100);
      caliAngleRight();
  delay(2000);
      rotateLeft(90);
  delay(2000);
      goStraightInGrids(1);
      delay(100);
  delay(2000);
      rotateRight(90);
      goStraightInGrids(1);
      delay(100);
  delay(2000);
      caliAngleFrontCR();
      delay(100);
      caliDistanceFront();
   */
  // Movement Check
  //goStraightInGrids(1);
  //delay(100);
  //rotateLeft(90);
  //rotateRight(90);

  //Calibration Angle Check
 // caliAngleFrontLR();
 // caliAngleFrontLC();
 //caliAngleFrontCR();
 //caliAngleRight();
 //delay(100);

 // Calibration Disance Check
 //caliDistanceFront();
  //delay(1000);


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
      goStraightInGrids(1);
      delay(50);
      //calibration_counter++;
      //if(//calibration_counter ==5){
      //rotateRight(90);
      //delay(100);
      //caliDistanceFront();
      //delay(100);
      //rotateLeft(90);
      //delay(100);
      //calibration_counter = 0;
      //}
      specialCalibrate();
      delay(50);
      sendSensorReading();
      //Serial.println("k");
      
      break;
    case '2':
      goStraightInGrids(2);
      
      break;
    case '3':
      goStraightInGrids(3);
      
      break;
    case '4':
      goStraightInGrids(4);
      
      break;
    case '5':
      goStraightInGrids(5);
      
      break;
    case '6':
      goStraightInGrids(6);
     
      break;
    case '7':
      goStraightInGrids(7);
     
      break;
    case '8':
      goStraightInGrids(8);
      
      break;
    case '9':
      goStraightInGrids(9);
      
      break;
    case '0':
      goStraightInGrids(10);
      
      break;

    case 'L':
      //md.setSpeeds(100, -100);
      rotateLeft(90);
      delay(50);
      specialCalibrate();
      delay(50);
      sendSensorReading();
      //Serial.println("L ok");
      break;
    case 'R':
      //md.setSpeeds(-100, 100);
      rotateRight(90);
      delay(50);
      specialCalibrate();
      delay(50);
      sendSensorReading();
      //Serial.println("R ok");
      break;

    case 'S':
      sendSensorReading();
      //Serial.println("S ok");
      break;


    case 'C':
      rotateRight(90);
      rotateRight(90);
      delay(200);
      caliAngleRight();
      delay(200);
      caliDistanceFrontR();
      delay(100);
      caliAngleRight();
      delay(200);
      rotateRight(90);
      delay(200);
      caliAngleFrontLR();
      delay(200);
      caliDistanceFrontR();
      delay(200);
      caliAngleFrontLR();
      delay(200);
      rotateRight(90);
      delay(200);
      Serial.println("k");
      break;
    case 'X':
      // delay(100);
      caliDistanceFrontR();
      delay(50);
      //sendSensorReading();
      Serial.println("k");
      break;
    case 'Y':
      //      delay(100);
      caliAngleRight();
      delay(50);
      //sendSensorReading();
      Serial.println("k");
      break;
    case 'O':
      //      delay(100);
      caliAngleFrontLR();
      delay(50);
      //sendSensorReading();
      front_calibration_counter = 0;
      Serial.println("k");
      break;

    case 'P':
      //      delay(100);
      caliAngleFrontLC();
      delay(50);
      //sendSensorReading();
      front_calibration_counter = 0;
      Serial.println("k");
      break;

    case 'Q':
      //      delay(100);
      caliAngleFrontCR();
      delay(50);
      //sendSensorReading();
      front_calibration_counter = 0;
      Serial.println("k");
      break;
    
  }
}
//================================================Right turn and front calibration===========================================================
void specialCalibrate(){
  front_calibration_counter++;
  if(front_calibration_counter >= 5){
    String rf = calculateGrids(RF);
    String rb = calculateGrids(RB);
    if(rf == "1" && rb == "1"){
      rotateRight(90);
      delay(50);
      String fl = calculateGrids(FL);
      String fc = calculateGrids(FC);
      String fr = calculateGrids(FR);
      if(fl == "1" && fc == "1"){
         caliAngleFrontLC();
         delay(100);
         caliDistanceFrontL();
         delay(100);
         caliAngleFrontLC();
         delay(100);
         front_calibration_counter = 0;
      }
      
      else if(fr == "1" && fc == "1"){
         caliAngleFrontCR();
         delay(100);
         caliDistanceFrontR();
         delay(100);
         caliAngleFrontCR();
         delay(100);
         front_calibration_counter = 0;
      }
      rotateLeft(90); 
    }
  }
    
}

//================================================Send distance to RPI========================================================================

void sendSensorReading()
{
  for (int i = 0; i < 5; i++)
  {
    gridsToRpi = gridsToRpi + "SDATA;1_" + calculateGrids(FL) + ";2_" + calculateGrids(FC) + ";3_" + calculateGrids(FR) + ";4_" + calculateGrids(RF) + ";5_" + calculateGrids(RB) + ";6_" + calculateGrids(LL) + ";";
    delay(100);
  }

  Serial.println(gridsToRpi);
  gridsToRpi = "";
}


String calculateGrids(int sensor)
{
  int k, n;
  double reading;
  double sum = 0;
  switch (sensor) {
    case FL:
      /*
        for (int i = 0; i < 29; i++)
        {
        Distance[i] = fl.distance();
        }
      */
      reading = fl.distance();
      if (reading >= 7 && reading <= 17)
        return "1";
      else if (reading >= 18 && reading <= 28)
        return "2";
      else return "-1";
    case FC:
      /*
        for (int i = 0; i < 29; i++)
        {
          Distance[i] = fc.distance();
        }
      */
      //reading =  Distance[29 / 2];
      reading = fc.distance();
      if (reading >= 7 && reading <= 16)
        return "1";
      else if (reading >= 17 && reading <= 28)
        return "2";
      else return "-1";
    case FR:
      /*
        for (int i = 0; i < 29; i++)
        {
          Distance[i] = fr.distance();
        }
      */
      //reading =  Distance[29 / 2];
      reading = fr.distance();
      if (reading >= 7 && reading <= 17)
        return "1";
      else if (reading >= 18 && reading <= 28)
        return "2";
      else return "-1";
    case LL:

      /*
        for (int i = 0; i < 29; i++)
        {
        Distance[i] = ll.distance();
        }
      */

      //reading =  Distance[29 / 2];
      reading = ll.distance();
      if (reading >= 17 && reading <= 22)
        return "1";
      else if (reading >= 23 && reading <= 29)
        return "2";
      else if (reading >= 30 && reading <= 40 )
        return "3";
      else return "-1";
    case RF:/*
      for (int i = 0; i < 9; i++)
      {
        Distance[i] = rf.distance();
      }
*/

      //reading =  Distance[9 / 2];
      reading = rf.distance();
      if (reading >= 7  && reading <= 16)
        return "1";
      else if (reading >= 17 && reading <= 28)
        return "2";
      else return "-1";
    case RB:
      /*
        for (int i = 0; i < 29; i++)
        {
          Distance[i] = rb.distance();
        }
      */


      //reading =  Distance[29 / 2];
      reading = rb.distance();
      if (reading >= 7 && reading <= 15)
        return "1";
      else if (reading >= 16 && reading <= 27)
        return "2";
      else return "-1";
  }
}


/*
  void insertionSort(double array[], int length)
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
*/


//========================================================Calibration==========================================================
void caliAngleFrontLR()
{
  float flDistance, frDistance, error;
  int counter = 0;
  flDistance = sensorCaliDistance(FL) + LR_OFFSET;
  frDistance = sensorCaliDistance(FR);
  error = flDistance - frDistance;


  while (abs(error) > 0.1 && counter < 30)
  {
    //Serial.println(error);
    if (error > 0)
    {
      md.setSpeeds(-100, 100);
      delay(abs(error * 40));
      md.setBrakes(300, 300);
    }
    else
    {
      md.setSpeeds(100, -100);
      delay(abs(error * 40));
      md.setBrakes(300, 300);
    }
    delay(10);
    flDistance = sensorCaliDistance(FL) + LR_OFFSET;
    frDistance = sensorCaliDistance(FR);
    error = flDistance - frDistance;
    //Serial.println(error);
    counter++;

  }

}

void caliAngleFrontCR()
{
  float fcDistance, frDistance, error;
  int counter = 0;
  fcDistance = sensorCaliDistance(FC) + CR_OFFSET;
  frDistance = sensorCaliDistance(FR);
  error = fcDistance - frDistance;

  if(calculateGrids(FC) == '1')
  {
  while (abs(error) > 0.2 && counter < 30)
  {
    //Serial.println(error);
    if (error > 0)
    {
      md.setSpeeds(-100, 100);
      delay(abs(error * 40));
      md.setBrakes(300, 300);
    }
    else
    {
      md.setSpeeds(100, -100);
      delay(abs(error * 40));
      md.setBrakes(300, 300);
    }
    delay(10);
    fcDistance = sensorCaliDistance(FC) + CR_OFFSET;
    frDistance = sensorCaliDistance(FR);
    error = fcDistance - frDistance;
    //Serial.println(error);
    counter++;

  }
  }

}

void caliAngleFrontLC()
{
  float flDistance, fcDistance, error;
  int counter = 0;
  flDistance = sensorCaliDistance(FL) + LC_OFFSET;
  fcDistance = sensorCaliDistance(FC);
  error = flDistance - fcDistance;

  if(calculateGrids(FC) == '1')
  {
  while (abs(error) > 0.2 && counter < 30)
  {
    
    //Serial.println(error);
    if (error > 0)
    {
      md.setSpeeds(-100, 100);
      delay(abs(error * 40));
      md.setBrakes(300, 300);
    }
    else
    {
      md.setSpeeds(100, -100);
      delay(abs(error * 40));
      md.setBrakes(300, 300);
    }
    delay(10);
    flDistance = sensorCaliDistance(FL) + LC_OFFSET;
    fcDistance = sensorCaliDistance(FC);
    error = flDistance - fcDistance;
    //Serial.println(error);
    counter++;

  }
  }

}

void caliAngleRight()
{
  float rfDistance, rbDistance;
  float error;
  int counter = 0;
  rfDistance = sensorCaliDistance(RF) + R_OFFSET;
  rbDistance = sensorCaliDistance(RB);
  error = rfDistance - rbDistance;

  while (abs(error) > ERROR_RIGHT && counter < 30)
  {
    if (error > 0)
    {
      md.setSpeeds(-100, 100);
      delay(abs(error * 40));
      md.setBrakes(300, 300);
    }
    else
    {
      md.setSpeeds(100, -100);
      delay(abs(error * 40));
      md.setBrakes(300, 300);
    }
    delay(10);
    rfDistance = sensorCaliDistance(RF) + R_OFFSET;
    rbDistance = sensorCaliDistance(RB);
    error = rfDistance - rbDistance;
    counter ++;
  }

}

void caliAngleRightFront()
{
  float rfDistance, frDistance;
  float error;
  int counter = 0;
  rfDistance = sensorCaliDistance(RF) + RF_OFFSET;
  frDistance = sensorCaliDistance(FR);
  error = rfDistance - frDistance;

  while (abs(error) > ERROR_RIGHT && counter < 30)
  {
    if (error > 0)
    {
      md.setSpeeds(-100, 100);
      delay(abs(error * 20));
      md.setBrakes(400, 400);
    }
    else
    {
      md.setSpeeds(100, -100);
      delay(abs(error * 20));
      md.setBrakes(400, 400);
    }
    delay(10);
    rfDistance = sensorCaliDistance(RF) + R_OFFSET;
    frDistance = sensorCaliDistance(FR);
    error = rfDistance - frDistance;
    counter ++;
  }

}
void caliDistanceFrontR()
{
  float targetDistance = 5 + CALI_DISTANCE_OFFSET_R;
  float actualDistance = sensorCaliDistance(FR);
  float difference;
  int counter = 0;
  difference = targetDistance - actualDistance;
  while (abs(difference) > 0.1 && counter < 30)
  {
    if (difference > 0)
    {
      //moveBackwardCali(); //move forward for 0.2cm
      md.setSpeeds(150, 150);
      delay(abs(difference * 20));
      md.setBrakes(400, 400);
    }
    else
    {
      md.setSpeeds(-150, -150);
      delay(abs(difference * 20));
      md.setBrakes(400, 400); //move backward for 0.2cm
    }
    delay(10);
    actualDistance = sensorCaliDistance(FR);
    difference = targetDistance - actualDistance;
    counter ++;
  }

}

void caliDistanceFrontL()
{
  float targetDistance = 5 + CALI_DISTANCE_OFFSET_L;
  float actualDistance = sensorCaliDistance(FL);
  float difference;
  int counter = 0;
  difference = targetDistance - actualDistance;
  while (abs(difference) > 0.1 && counter < 30)
  {
    if (difference > 0)
    {
      //moveBackwardCali(); //move forward for 0.2cm
      md.setSpeeds(150, 150);
      delay(abs(difference * 20));
      md.setBrakes(300, 300);
    }
    else
    {
      md.setSpeeds(-150, -150);
      delay(abs(difference * 20));
      md.setBrakes(300, 300); //move backward for 0.2cm
    }
    delay(10);
    actualDistance = sensorCaliDistance(FL);
    difference = targetDistance - actualDistance;
    counter ++;
  }

}


float sensorCaliDistance(int sensor)
{
  float Distance, voltage, middle;
  switch (sensor)
  {
    case FL:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(FL);
      }
      //Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      middle = median(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = middle / 1023 * 5;

      //Distance = -2.0927 * pow(voltage, 3) + 16.475 * voltage * voltage -47.533 * voltage + 52.481;
      Distance = -6.3295 * pow(voltage, 3) + 45.689 * voltage * voltage - 113.84 * voltage + 101.02;
      //distance =
      return Distance;

    case FR:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(FR);
      }
      //Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      middle = median(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = middle / 1023 * 5;
      //Distance = -4.4405 * pow(voltage, 3) + 30.795 * voltage * voltage -76.011 * voltage + 70.041;
      Distance = -1.7829 * pow(voltage, 3) + 14.583 * voltage * voltage - 43.971 * voltage + 49.982;
      //distance =
      return Distance;
    case RF:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(RF);
      }
      middle = median(caliReading, NUM_SENSOR_READINGS_CALI);
      //Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = middle / 1023 * 5;
      Distance = -1.7829 * pow(voltage, 3) + 14.583 * voltage * voltage + -43.971 * voltage + 49.982;
      //distance =
      return Distance;
    case RB:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(RB);
      }
      middle = median(caliReading, NUM_SENSOR_READINGS_CALI);
      //Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = middle / 1023 * 5;
      Distance = -3.3307 * pow(voltage, 3) + 24.122 * voltage * voltage - 62.573 * voltage + 63.155; //doenst works when distance less than 2.5cm
      //distance =
      return Distance;
    case FC:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(FC);
      }
      middle = median(caliReading, NUM_SENSOR_READINGS_CALI);
      //Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = middle / 1023 * 5;
      Distance = -1.6918 * pow(voltage, 3) + 13.651 * voltage * voltage - 40.899 * voltage + 47.82; //doenst works when distance less than 2.5cm
      //distance =
      return Distance;

  }

}
float median(float nums[], int n) {
  int k = n % 2 == 0 ? n / 2 : n / 2 + 1;
  return qselect(nums, 0, n - 1, k - 1);
}

float qselect(float A[], int start, int end, int k) {
  if (start == end) {
    return A[start];
  }

  int left = start, right = end;
  int index = (left + right) / 2;
  float pivot = A[index];

  while (left <= right) {
    while (left <= right && A[left] > pivot) {
      left++;
    }

    while (left <= right && A[right] < pivot) {
      right--;
    }

    if (left <= right) {
      float tmp = A[left];
      A[left] = A[right];
      A[right] = tmp;

      left++;
      right--;
    }
  }

  if (k >= start && k <= right) {
    return qselect(A, start, right, k);
  }
  if (k >= left && k <= end) {
    return qselect(A, left, end, k);
  }


  return A[right + 1];
}

void Sort(int array[], int length)//sort integers
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

void Brake() {
  md.setBrakes(400, 400);
}

void goStraightInGrids(long grids) {
  long distance = grids * 9700;//9800;//9900;//10100;//10200;//10300//10500;//10800;

  while (true) {
    if (total_Dis >= distance) {
      total_Dis = 0;
      md.setBrakes(400, 400);
      break;
    } else {
      moveForward();
      //Serial.print(input_MR); //Serial.print(", R  ||  L ,");
      //Serial.print("R || L");
      //Serial.println(input_ML); //Serial.print(", L\n");
      total_Dis = total_Dis + input_ML + input_MR;
    }
  }
  delay(50);
  restartPID();
}

void goStraightInGridsFast(long grids) {
  long distance = grids * 11175;
  while (true) {
    if (total_Dis >= distance) {
      total_Dis = 0;
      md.setBrakes(400, 400);
      break;
    } else {
      moveForward();
      //Serial.print(input_MR); Serial.print(", R  ||  ");
      //Serial.print(input_ML); Serial.print(", L\n");
      total_Dis = total_Dis + input_ML + input_MR;
    }
  }
  delay(50);
  restartPID();
}

void goBackInGrids(long grids) {
  long distance = grids * 10100;
  while (true) {
    if (total_Dis >= distance) {
      total_Dis = 0;
      md.setBrakes(400, 400);
      break;
    } else {
      moveBackward();
      //Serial.print(input_MR); Serial.print(", R  ||  ");
      //Serial.print(input_ML); Serial.print(", L\n");
      total_Dis = total_Dis + input_ML + input_MR;
    }
  }
  delay(50);
  restartPID();
}




// ==============================   Motor Functions   ==============================
void countPulse_MR() {
  counter_MR++;
  right_encoder_val++;
  if (counter_MR == 1) startTime_MR = micros();
  else if (counter_MR == 11) {
    endTime_MR = micros();
    duration_MR = (endTime_MR - startTime_MR) / 10.0;
    input_MR = calculateRPM(duration_MR);
    counter_MR = 0;
  }
}

void countPulse_ML() {
  counter_ML++;
  left_encoder_val2++;
  left_encoder_val++;
  if (counter_ML == 1) startTime_ML = micros();
  else if (counter_ML == 11) {
    endTime_ML = micros();
    duration_ML = (endTime_ML - startTime_ML) / 10.0;
    input_ML = calculateRPM(duration_ML);
    counter_ML = 0;
  }
}

double calculateRPM(double pulse) {
  if (pulse == 0) return 0;
  else return 60.00 / (pulse *  562.25 / 1000000.00);
}


void pidCalculation(float kp_MR, float ki_MR, float kd_MR, float kp_ML, float ki_ML, float kd_ML, double setpoint) {

  // calculate k1, k2, k3 for both motors
  double k1_MR = kp_MR + ki_MR + kd_MR;
  double k2_MR = -kp_MR - 2 * kd_MR;
  double k3_MR = kd_MR;

  double k1_ML = kp_ML + ki_ML + kd_ML;
  double k2_ML = -kp_ML - 2 * kd_ML;
  double k3_ML = kd_ML;

  // calculate error values
  error_MR = (setpoint - input_MR) / 130.0; //150
  error_ML = (setpoint - input_ML) / 130.0; //150

  // compute PID controller output
  pidOutput_MR = prev_pidOutput_MR + k1_MR * error_MR + k2_MR * prev_error_MR + k3_MR * prev_prev_error_MR;
  pidOutput_ML = prev_pidOutput_ML + k1_ML * error_ML + k2_ML * prev_error_ML + k3_ML * prev_prev_error_ML;

  // save current data into memory
  prev_prev_error_MR = prev_error_MR;
  prev_prev_error_ML = prev_error_ML;
  prev_error_MR = error_MR;
  prev_error_ML = error_ML;
  prev_pidOutput_MR = pidOutput_MR;
  prev_pidOutput_ML = pidOutput_ML;
}

void restartPID() {
  prev_prev_error_MR = prev_prev_error_ML = 0;
  prev_error_MR = prev_error_ML = 0;
  error_MR = error_ML = 0;
  prev_pidOutput_MR = prev_pidOutput_ML = 0;
  pidOutput_MR = pidOutput_ML = 0;
  input_MR = input_ML = 0;
}

void moveForward() {
  /*if (initial) {
    pidCalculation(kp_MR, ki_MR, kd_MR, kp_ML, ki_ML, kd_ML, setpoint);
    md.setSpeeds(-pidOutput_ML * initial_Speed, -pidOutput_MR * initial_Speed);
  }*/
  pidCalculation(kp_MR, ki_MR, kd_MR, kp_ML, ki_ML, kd_ML, setpoint);
  md.setSpeeds(-pidOutput_ML * 150, -pidOutput_MR * 150);



  //Serial.print(input_ML);
  //Serial.print(" L || R ");
  //Serial.print(",");
  //Serial.println(input_MR);

  delayMicroseconds(5000);
}

void moveForwardSlow() {
  pidCalculation(kp_MR, ki_MR, kd_MR, kp_ML, ki_ML, kd_ML, setpoint / 2);
  md.setSpeeds(pidOutput_MR * 150, -pidOutput_ML * 150);
  delayMicroseconds(5000);
}

void moveBackward() {
  pidCalculation(kp_MR, ki_MR, kd_MR, kp_ML, ki_ML, kd_ML, setpoint);
  md.setSpeeds(-pidOutput_MR * 150, pidOutput_ML * 150);
  delayMicroseconds(5000);
}

void moveBackwardSlow() {
  pidCalculation(kp_MR, ki_MR, kd_MR, kp_ML, ki_ML, kd_ML, setpoint / 2);
  md.setSpeeds(-pidOutput_MR * 150, pidOutput_ML * 150);
  delayMicroseconds(5000);
}

// ================ ROTATION =====================
void rotateLeft(int degree)
{
  float output;
  float dis = (degree + LEFT_ROTATION_DEGREE_OFFSET) / 90.0;
  int left_speed = 380;
  int right_speed = 380;
  float actual_distance = (dis * 405);

  delay(1);
  md.setSpeeds(left_speed, -right_speed);
  resetCounter();
  while (left_encoder_val2 < actual_distance) {
    //output = pidControlForward(left_encoder_val, right_encoder_val);
    delay(1);
    //Serial.println(output);
    //Serial.println(left_encoder_val2);
    md.setSpeeds(left_speed, -right_speed);
    if (left_encoder_val2 > actual_distance)
    {
      break;
    }
  }
  md.setBrakes(400, 400);
  
}

void rotateRight(int degree)
{
  float output;
  float dis = (degree + RIGHT_ROTATION_DEGREE_OFFSET) / 90.0;
  int left_speed = 380;
  int right_speed = 380;
  float actual_distance = (dis * 405);

  delay(1);
  md.setSpeeds(-left_speed, right_speed);
  resetCounter();
  while (left_encoder_val2 < actual_distance) {
    //output = pidControlForward(left_encoder_val, right_encoder_val);
    delay(1);
    //Serial.println(output);
    md.setSpeeds(-left_speed, right_speed);
    if (left_encoder_val2 > actual_distance)
    {
      break;
    }
  }
  md.setBrakes(400, 400);
  
}
