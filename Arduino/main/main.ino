#include <DualVNH5019MotorShield.h>
#include <SharpIR.h>
#include <PinChangeInt.h>
#include <RunningMedian.h>

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
#define NUM_SENSOR_READINGS_CALI 19

#define CALI_DISTANCE_OFFSET_R -0.1//-0.3//-0.2//-0.1//-0.7//0.3 //offset for distance calibration using front sensor
#define CALI_DISTANCE_OFFSET_L 1.4//1.3//1.5//1//0.6//0.3//0//-0.2//-0.4//-0.5//-0.6//-0.7//-1.1//-0.9//-0.7//-0.9//-0.7//-0.8//-0.4//-0.2//-0.1//0//0.1//0  //too far: increase
#define LR_OFFSET 0.78//0.75//0.8//0.85//0.9//1.2//1.1//0.5//0.9 //offset for angle calibration using FR and FL sensor
#define LC_OFFSET 1.2//1.1//0.9//0.85//1//0.7//1.2 //offset for angle calibration using FL and FC sensor
#define CR_OFFSET -0.4 //offset for angle calibration using FR and FC sensor
#define R_OFFSET 2.68//2.6//2.5//2.4//2.68//1.8//1.9//1.8//1.2////1.4//1.6//1.4//1.1//-0.5 //offset for angle calibration using right sensors
#define RF_OFFSET 0.5

#define thresholdRF 0.7//1.9//1.8//1.6//1.4//0.9//0.59//0.4//0.49//0.56
#define thresholdRB 2.04

#define thresholdRF_LONG 6.25
#define thresholdRB_LONG 8.56

#define CALI_ANGLE_COUNTER 3
#define CALI_DISTANCE_COUNTER 3

int front_calibration_counter = 0;


#define ERROR_RIGHT 0
#define ERROR_FRONT 0
int maxCalibrationTrial = 30;

float caliReading[50]; //sensor readings for calibration

String gridsToRpi = "";

//========================= for rotation ====================================
#define motor_R_enconder 11
#define motor_L_enconder 3

volatile int left_encoder_val = 0, right_encoder_val = 0, left_encoder_val2 = 0;

float error = 0.0;
float prevError = 0.0;
float integral = 0.0;
float derivative;
float output = 0.0;

float testSpeed = 320; //180                              //
float PID_KP = 1.6; // lower = right, higher = left     // computePID() //1.6
float PID_KI = 0.26;//0.27;//0.25;//0.2;//0.3;//0.5; //0.06; //0.00222;//0.01                                       // computePID()
float PID_KD = 0; //0.05

float PID_KP_R = 1.6; // lower = right, higher = left     // computePID() //1.6
float PID_KI_R = 0.3;//0.2;//0.3;//0.5; //0.06; //0.00222;//0.01
float PID_KD_R = 0;// computePID()
//float GRID_DISTANCE[10] = {10.15, 20.3, 29.7, 40.1, 51, 60, 70, 80, 90, 100};  // moveByGrids(int)
//float DG_GRID_DISTANCE[5] = {14.7, 20.5, 29.7, 40.1, 51}; // moveByDgGrids(int)
float LEFT_ROTATION_DEGREE_OFFSET = -5.0;//-4.9;//-4.6;//-4.5;//-6.5;//-4.5                   // rotateLeft(int)
float RIGHT_ROTATION_DEGREE_OFFSET =-8.0;//-8.4;//-8.6;//-8.8;// -9.0;//-9.2; //-9.2; //for 6.44V battery
//-9.2;//-9.5;//-9.1;//-8.9;//-8.5;//-8.3;//-7.5;//-7.9;//-7.7;//-7.5;//-8.5;//-7.0;//-7.5;//-8.5;//-8.2;//-8.3;//-8.8;////-7.8//-3.8;   //-2.8               // rotateRight(int)
//int NUM_OF_SENSOR_READINGS_TO_TAKE = 15;                  // getDistance()
//int NUM_OF_SENSOR_READINGS_TO_TAKE_CALIBRATION = 5;       // calAngle() and calDistance()
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
float ki_MR = 0.13;//0.125;//0.13;//0.125;//0.12;//0.14;//0.13;//0.11;//0.15;//0.13;//0.15;//0.1;//0.12;//0.1;//0.2;//0.1;//0.10;//0.0800;
float kd_MR = 0;//0;//0.160;
//float kp_ML = 0.110;
// NOTE MOTOR_LEFT (Purple Line)
float kp_ML = 1.5;//0.085;//0.085;//0.0100;//0.025;//0.065;//0.075;//0.11;
float ki_ML = 0.13;//0.135;//0.13; //0.14;//0.12;// 0.11;//0.12;//0.15;//0.1;//0.10;//0.09;//0.065;//0.080;
float kd_ML = 0;//0.160;//0.014;


float kp_MR_Fast = 2.6;//2.4;//2.2;//0.085;
float ki_MR_Fast = 0.135;//0.13;//0.125;//0.13;//0.125;//0.12;//0.14;//0.13;//0.11;//0.15;//0.13;//0.15;//0.1;//0.12;//0.1;//0.2;//0.1;//0.10;//0.0800;
float kd_MR_Fast = 0;//0;//0.160;
//float kp_ML = 0.110;
// NOTE MOTOR_LEFT (Purple Line)
float kp_ML_Fast = 1.5;//0.085;//0.085;//0.0100;//0.025;//0.065;//0.075;//0.11;
float ki_ML_Fast = 0.13; //0.14;//0.12;// 0.11;//0.12;//0.15;//0.1;//0.10;//0.09;//0.065;//0.080;
float kd_ML_Fast = 0;//0.160;//0.014;

double setpoint = 130;//90//80
double setpoint_RT = 100;

float offsetR =1, offsetL = 1;

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

  //============================Preparation here====================================
  /*
   * fully charge the two batteries, fully charge the power bank
   * drain one battery and use this battery to test
   * drain another battery and use this battery to run
   * fully calibration, rotateLeft for 2 rounds
   * fully calibration, rotateRight for 2 rounds
   * fully calibration, moveForward for 5 steps
   * test specialDistanceCalibration
   * test sensor reading
   * goStraightInGridsFast for 8 grids (now slightly towards right, if really cannot, make it slightly to left), rotateRight, goStraightInGridsFast for 3 grids
   * 
   */
  //#RUN1 calibration + 2 rounds left
//      rotateRight(90);
//      rotateRight(90);
//      delay(200);
//      caliAngleRight();
//      delay(200);
//      caliDistanceFrontR();
//      delay(100);
//      caliAngleRight();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      //caliAngleFrontLR();
//      //delay(200);
//      caliDistanceFrontR();
//      delay(200);
//      caliAngleFrontLR();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      for(int i = 0; i<8; i++)
//      {
//        rotateLeft(90);
//        delay(100);
//      }
//
//      while(1)
//      {
//        delay(10000);
//      }


//#RUN2 calibration + 2 rounds right 
//      rotateRight(90);
//      rotateRight(90);
//      delay(200);
//      caliAngleRight();
//      delay(200);
//      caliDistanceFrontR();
//      delay(100);
//      caliAngleRight();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      //caliAngleFrontLR();
//      //delay(200);
//      caliDistanceFrontR();
//      delay(200);
//      caliAngleFrontLR();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      for(int i = 0; i<4; i++)
//      {
//        rotateLeft(90);
//        delay(1000);
//      }
//
//      while(1)
//      {
//        delay(10000);
//      }

//#RUN2 calibration + 2 rounds right 
//      rotateRight(90);
//      rotateRight(90);
//      delay(200);
//      caliAngleRight();
//      delay(200);
//      caliDistanceFrontR();
//      delay(100);
//      caliAngleRight();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      //caliAngleFrontLR();
//      //delay(200);
//      caliDistanceFrontR();
//      delay(200);
//      caliAngleFrontLR();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      for(int i = 0; i<4 i++)
//      {
//        rotateRight(90);
//        delay(100);
//      }
//
//      while(1)
//      {
//        delay(10000);
//      }



//#RUN3 calibration + moveForward 4 steps
//      rotateRight(90);
//      rotateRight(90);
//      delay(200);
//      caliAngleRight();
//      delay(200);
//      caliDistanceFrontR();
//      delay(100);
//      caliAngleRight();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      //caliAngleFrontLR();
//      //delay(200);
//      caliDistanceFrontR();
//      delay(200);
//      caliAngleFrontLR();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      for(int i = 0; i<4; i++)
//      {
//        goStraightInGrids(1);
//        delay(100);
//      }
//
//      while(1)
//      {
//        delay(10000);
//      }
//goStraightInGrids(1);
//#RUN4 calibration + moveForward 4 steps
//      rotateRight(90);
//      rotateRight(90);
//      delay(200);
//      caliAngleRight();
//      delay(200);
//      caliDistanceFrontR();
//      delay(100);
//      caliAngleRight();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      //caliAngleFrontLR();
//      //delay(200);
//      caliDistanceFrontR();
//      delay(200);
//      caliAngleFrontLR();
//      delay(200);
//      rotateRight(90);
//      delay(200);
//      for(int i = 0; i<4; i++)
//      {
//        goStraightInGrids(1);
//        delay(100);
//      }
//
//      while(1)
//      {
//        delay(10000);
//      }

  if (Serial.available() > 0)
  {
    commandExecution(char(Serial.read()));
  }
  

//14 //l
//11 //c
//14 //r

 // Serial.println(fl.distance());

 
// goStraightInGridsFast(8);
// delay(10);
// rotateRight(90);
// delay(10);
// goStraightInGridsFast(3);
// while(1)
// {
//  delay(10000);
// }
//
// goStraightInGridsFast(8);
// delay(10);
// rotateRight(90);
// delay(10);
//  goStraightInGridsFast(3);
// while(1)
// {
//  delay(10000);
// }

/*

  float rfDistance, rbDistance, rfDistance_, rbDistance_;
  float error;
  int counter = 0;
  rfDistance = sensorCaliDistance(RF) + R_OFFSET;
  rbDistance = sensorCaliDistance(RB);
  error = rfDistance - rbDistance;
  Serial.println(error);
*/
    
//  
// Serial.print(sensorCaliDistance(FL));
// Serial.print(",");
// Serial.println(sensorCaliDistance(FC));


//   front_calibration_counter = 5;
//   specialCalibrate();
//   while(1)
//   {
//    delay(10000);
//   }

   //caliAngleFrontLC();
   /*
   front_calibration_counter = 5;
   specialCalibrate();
   while(1)
   {
    delay(10000);
   }*/
   //Serial.println(sensorCaliDistance(RF));
   //Serial.print(",");

   //Serial.println(sensorCaliDistance(RB));

/*
    for(int i = 0; i<8; i++)
    {
    //rotateRight(90);
    goStraightInGrids(1);
    //Serial.println("inside the loop");
    delay(1000);
    }
    while(1)
    {
    delay(10000);
    }
   */

   
  //caliFront();

  //caliAngleRight();
  //Serial.println(ll.distance());
    
}

void commandExecution(char cmd)
{
  switch (cmd) {
    case 'F':
      goStraightInGrids(1);
      delay(100);
      specialCalibrate();
      specialDistanceCalibration();
      sendSensorReading();
      break;

    case '1':
      goStraightInGridsFast(1);
      Serial.println("k");
      break;
    case '2':
      goStraightInGridsFast(2);
      Serial.println("k");
      break;
    case '3':
      goStraightInGridsFast(3);
      Serial.println("k");
      break;
    case '4':
      goStraightInGridsFast(4);
      Serial.println("k");
      break;
    case '5':
     goStraightInGridsFast(5);
      Serial.println("k");
      break;
    case '6':
      goStraightInGridsFast(6);
      Serial.println("k");
      break;
    case '7':
      goStraightInGridsFast(7);
      Serial.println("k");
      break;
    case '8':
      goStraightInGridsFast(8);
      Serial.println("k");
      break;
    case '9':
      goStraightInGridsFast(9);
      Serial.println("k");
      break;
    case '0':
      goStraightInGridsFast(10);
      Serial.println("k");
      break;

    case 'L':
      rotateLeft(90);
      delay(100);
      specialCalibrate();
      specialDistanceCalibration();
      sendSensorReading();
      break;
    case 'A':
      rotateLeft(90);
      Serial.println("k");
      break;

    case 'R':
      rotateRight(90);
      delay(100);
      specialCalibrate();
      specialDistanceCalibration();
      sendSensorReading();
      break;
    case 'D':
      rotateRight(90);
      Serial.println("k");
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
      //caliAngleFrontLR();
      //delay(200);
      caliDistanceFrontR();
      delay(200);
      caliAngleFrontLR();
      delay(200);
      rotateRight(90);
      delay(200);
      Serial.println("k");
      break;
    case 'X':
      caliDistanceFrontR();
      Serial.println("k");
      break;
    case 'Y':
      caliAngleRight();
      Serial.println("k");
      break;
    case 'O':
      caliAngleFrontLR();
      front_calibration_counter = 0;
      Serial.println("k");
      break;
    case 'P':
      caliAngleFrontLC();
      front_calibration_counter = 0;
      Serial.println("k");
      break;
    case 'Q':
      caliAngleFrontCR();
      front_calibration_counter = 0;
      Serial.println("k");
      break;

    case 'N':
      caliDistanceFrontR();
      Serial.println("k");
      break;
    case 'V':
      caliDistanceFrontL();
      Serial.println("k");
      break;
    case 'S':
      sendSensorReading();
      //Serial.println("S ok");
      break;
  }
}
//================================================Right turn and front calibration===========================================================
void specialCalibrate() {
  front_calibration_counter++;
  if (front_calibration_counter >= 5) {
    String rf = calculateGrids(RF);
    String rb = calculateGrids(RB);
    if (rf == "1" && rb == "1") {
      rotateRight(90);
      delay(100);
      String fl = calculateGrids(FL);
      String fc = calculateGrids(FC);
      String fr = calculateGrids(FR);
     

      if (fr == "1" && fc == "1") {
       // caliAngleFrontCR();
       // delay(100);
        caliDistanceFrontR();
        delay(100);
        caliAngleFrontCR();
        delay(100);
        front_calibration_counter = 0;
      }
       else if (fl == "1" && fc == "1") {
       // caliAngleFrontLC();
       // delay(100);
        caliDistanceFrontL();
        delay(100);
        caliAngleFrontLC();
        delay(100);
        front_calibration_counter = 0;
      }
      rotateLeft(90);
      delay(100);
    }
  }

}

//===============================================Front distance calibration=================================================================
void specialDistanceCalibration()
{
  String fr = calculateGrids(FR);
  String fl = calculateGrids(FL);
  
  if (fr == "1")
  {
    caliDistanceFrontR();
  }
  else if (fl == "1")
  {
    caliDistanceFrontL();
    
  }
  delay(100);
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
      reading = fl.distance();
      if (reading >= 7 && reading <= 17)
        return "1";
      else if (reading >= 18 && reading <= 28)
        return "2";
      else return "-1";
    case FC:
      //reading =  Distance[29 / 2];
      reading = fc.distance();
      if (reading >= 7 && reading <= 16)
        return "1";
      else if (reading >= 17 && reading <= 28)
        return "2";
      else return "-1";
    case FR:
      reading = fr.distance();
      if (reading >= 7 && reading <= 17)
        return "1";
      else if (reading >= 18 && reading <= 28)
        return "2";
      else return "-1";
    case LL:
      reading = ll.distance();
      if (reading >= 17 && reading <= 22)
        return "1";
      else if (reading >= 23 && reading <= 29)
        return "2";
      else if (reading >= 30 && reading <= 40 )
        return "3";
      else if (reading >= 41 && reading <= 52)
        return "4";
      else return "-1";
    case RF:
      reading = rf.distance();
      if (reading >= 7  && reading <= 16)
        return "1";
      else if (reading >= 17 && reading <= 28)
        return "2";
      else return "-1";
    case RB:
      reading = rb.distance();
      if (reading >= 7 && reading <= 15)
        return "1";
      else if (reading >= 16 && reading <= 27)
        return "2";
      else return "-1";
  }
}


//========================================================Calibration==========================================================
void caliFront() {
  String f_l = calculateGrids(FL);
  String f_r = calculateGrids(FR);
  String f_c = calculateGrids(FC);

  if (f_l == "1" && f_c == "1") {
    //caliAngleFrontLC();
    caliDistanceFrontL();
    caliAngleFrontLC();
    delay(100);
  } else if (f_c == "1" && f_r == "1") {
    //caliAngleFrontCR();
    caliDistanceFrontR();
    caliAngleFrontCR();
    delay(100);
  } else if (f_l == "1" && f_r == "1") {
    //caliAngleFrontLR();
    caliDistanceFrontL();
    caliAngleFrontLR();
    delay(100);
  }
}

void caliAngleFrontLR()
{
  float flDistance, frDistance, error;
  int counter = 0;
  flDistance = sensorCaliDistance(FL) + LR_OFFSET;
  frDistance = sensorCaliDistance(FR);
  error = flDistance - frDistance;

  String f_l = calculateGrids(FL);
  String f_r = calculateGrids(FR);

  if (f_l == "1" && f_r == "1")
  {
    while (abs(error) > 0.1 && counter < CALI_ANGLE_COUNTER)
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

}

void caliAngleFrontCR()
{
  float fcDistance, frDistance, error;
  int counter = 0;
  fcDistance = sensorCaliDistance(FC) + CR_OFFSET;
  frDistance = sensorCaliDistance(FR);
  error = fcDistance - frDistance;

  String f_c = calculateGrids(FC);
  String f_r = calculateGrids(FR);
  if (f_c == "1" && f_r == "1")
  {
    while (abs(error) > 0.2 && counter < CALI_ANGLE_COUNTER)
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
      fcDistance = sensorCaliDistance(FC) + CR_OFFSET;
      frDistance = sensorCaliDistance(FR);
      error = fcDistance - frDistance;
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

  String f_c = calculateGrids(FC);
  String f_l = calculateGrids(FL);
  if ( f_c == "1" && f_l == "1" )
  {


    while (abs(error) > 0.2 && counter < CALI_ANGLE_COUNTER)
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
      flDistance = sensorCaliDistance(FL) + LC_OFFSET;
      fcDistance = sensorCaliDistance(FC);
      error = flDistance - fcDistance;
      counter++;

    }
  }

}

void caliAngleRight()
{
  float rfDistance, rbDistance, rfDistance_, rbDistance_;
  float error;
  int counter = 0;
  rfDistance = sensorCaliDistance(RF) + R_OFFSET;
  rbDistance = sensorCaliDistance(RB);
  error = rfDistance - rbDistance;
  rfDistance_ = sensorCaliDistance(RF);
  rbDistance_ = sensorCaliDistance(RB);
  String r_f = calculateGrids(RF);
  String r_b = calculateGrids(RB);
  if (r_f == "1" && r_b == "1")
  {
    if(rfDistance_ < thresholdRF || rbDistance_ < thresholdRB || rfDistance_ > thresholdRF_LONG || rbDistance_ > thresholdRB_LONG)
    {
      front_calibration_counter = 5;
      specialCalibrate();
    }

    if(error > 5)
    {
      return;
    }
      while (abs(error) > ERROR_RIGHT && counter < CALI_ANGLE_COUNTER)
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
      delay(100);
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

  while (abs(error) > 0.1 && counter < CALI_ANGLE_COUNTER)
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
  if (calculateGrids(FR) == "1" )
  {
    while (abs(difference) > 0.1 && counter < CALI_DISTANCE_COUNTER)
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

}

void caliDistanceFrontL()
{
  
  float actualDistance = sensorCaliDistance(FL) + CALI_DISTANCE_OFFSET_L;
  float targetDistance = 5;
  float difference;
  int counter = 0;
  difference = targetDistance - actualDistance;
  if (calculateGrids(FL) == "1" )
  {
    while (abs(difference) > 0.1 && counter < CALI_DISTANCE_COUNTER)
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
      actualDistance = sensorCaliDistance(FL) + CALI_DISTANCE_OFFSET_L;
      difference = targetDistance - actualDistance;
      counter ++;
    }
  }

}


float sensorCaliDistance(int sensor)
{
  float Distance, voltage, middle;
  RunningMedian med = RunningMedian(NUM_SENSOR_READINGS_CALI);

  switch (sensor)
  {

    case FL:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        med.add(analogRead(FL));
      }
      //Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      middle = med.getMedian();
      voltage = middle / 1023 * 5;
      Distance = -6.3295 * pow(voltage, 3) + 45.689 * voltage * voltage - 113.84 * voltage + 101.02;
      //distance =
      return Distance;

    case FR:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        med.add(analogRead(FR));
      }
      middle = med.getMedian();
      voltage = middle / 1023 * 5;
      Distance = -1.7829 * pow(voltage, 3) + 14.583 * voltage * voltage - 43.971 * voltage + 49.982;
      //distance =
      return Distance;
    case RF:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        med.add(analogRead(RF));
      }
      middle = med.getMedian();
      voltage = middle / 1023 * 5;
      Distance = -1.7829 * pow(voltage, 3) + 14.583 * voltage * voltage + -43.971 * voltage + 49.982;
      return Distance;
    case RB:
      //RunningMedian med = RunningMedian(NUM_SENSOR_READINGS_CALI);
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        med.add(analogRead(RB));
      }
      middle = med.getMedian();
      voltage = middle / 1023 * 5;
      Distance = -3.3307 * pow(voltage, 3) + 24.122 * voltage * voltage - 62.573 * voltage + 63.155; //doenst works when distance less than 2.5cm
      return Distance;
    case FC:
      //RunningMedian med = RunningMedian(NUM_SENSOR_READINGS_CALI);
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        med.add(analogRead(FC));
      }
      middle = med.getMedian();
      voltage = middle / 1023 * 5;
      Distance = -1.6918 * pow(voltage, 3) + 13.651 * voltage * voltage - 40.899 * voltage + 47.82; //doenst works when distance less than 2.5cm
      return Distance;

  }

}
//======================================================Move forward or backward using PID===============================================================================
/*
  void lTick()  {
  left_encoder_val++;
  left_encoder_val2++;
  }

  void rTick()  {
  right_encoder_val++;
  }
*/
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


void goStraightInGrids(long grids) {
  long distance = grids * 9510;//9500;//9480;//9500;//9550;//9600;//9630;//9500;//9700;//9800;//9900;//10100;//10200;//10300//10500;//10800;

  while (true) {
    if (total_Dis >= distance) {
      total_Dis = 0;
      offsetL = 1;
      offsetR = 1;
      md.setBrakes(400, 400);
      break;
    }
    else {
      moveForward(distance);
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
      moveForward_Fast();
      total_Dis = total_Dis + input_ML + input_MR;
    }

    //checkFront()
    
  }
  delay(50);
  restartPID();
  }


  void checkFront(){
    
  }


/*
  void goBackInGrids(long grids) {
  long distance = grids * 10100;
  while (true) {
    if (total_Dis >= distance) {
      total_Dis = 0;
      md.setBrakes(400, 400);
      break;
    } else {
      moveBackward();
      total_Dis = total_Dis + input_ML + input_MR;
    }
  }
  delay(50);
  restartPID();
  }
*/
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


/*
  bool eBrake()
  {
  if (fc.distance() < BRAKE_FC || fl.distance() < BRAKE_FL || fr.distance() < BRAKE_FR)
  {
    md.setBrakes(400, 400);
    delay(100);
    caliAngleFrontLR();
    return true;
  }
  return false;

  }

*/
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

void moveForward(long distance) {
  /*if (initial) {
    pidCalculation(kp_MR, ki_MR, kd_MR, kp_ML, ki_ML, kd_ML, setpoint);
    md.setSpeeds(-pidOutput_ML * initial_Speed, -pidOutput_MR * initial_Speed);
    }*/
  pidCalculation(kp_MR, ki_MR, kd_MR, kp_ML, ki_ML, kd_ML, setpoint);
  /*
  if (total_Dis < distance - 200) //200 is randomly set, for trial only
  {
    md.setSpeeds(-pidOutput_ML * 150 * offsetL, -pidOutput_MR * 150 * offsetR);
    offsetL -= 0.05;
    offsetR -= 0.05;
  }
  else {
  */
  md.setSpeeds(-pidOutput_ML * 150, -pidOutput_MR * 150);
  delayMicroseconds(5000);
}

void moveForward_Fast() {
 // pidCalculation(kp_MR, ki_MR, kd_MR, kp_ML, ki_ML, kd_ML, setpoint);
  pidCalculation(kp_MR_Fast, ki_MR_Fast, kd_MR_Fast, kp_ML_Fast, ki_ML_Fast, kd_ML_Fast, setpoint);
  md.setSpeeds(-pidOutput_ML * 150, -pidOutput_MR * 150);
  delayMicroseconds(5000);
}

void moveForwardSlow() {
  //pidCalculation(kp_MR_Long, ki_MR_Long, kd_MR_Long, kp_ML_Long, ki_ML_Long, kd_ML_Long, setpoint / 2);
  pidCalculation(kp_MR, ki_MR, kd_MR, kp_ML, ki_ML, kd_ML, setpoint);
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
void rotateLeft(int degree) {
  float offset = 0.1;
  float output;
  float dis = (degree + LEFT_ROTATION_DEGREE_OFFSET) / 90.0;
  int left_speed = 300;
  int right_speed = 300;
  float actual_distance = (dis * 405);
  delay(1);
  md.setSpeeds(left_speed, -right_speed);
  resetPID();
  while (left_encoder_val2 < actual_distance) {
    output = pidControlForward(left_encoder_val, right_encoder_val);
    if (left_encoder_val2 <= 8)
    {
      md.setSpeeds(offset * left_speed, offset * (-right_speed + output));
      offset += 0.1;
      //Serial.println(left_speed*offset);
    }
    else if (actual_distance - left_encoder_val2 <= 8)
    {
      md.setSpeeds(offset * left_speed, offset * (-right_speed + output));
      offset -= 0.1;
      //Serial.println(left_speed*offset);
    }
    else
    {
      md.setSpeeds(left_speed, -right_speed + output);
      //Serial.println(left_speed);
    }
    
    
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
  int left_speed = 300;
  int right_speed = 300;
  float actual_distance = (dis * 405);
  float offset = 0.1;
  delay(1);
  md.setSpeeds(-left_speed, right_speed);
  resetPID();
  while (left_encoder_val2 < actual_distance) {
    output = pidControlForward(left_encoder_val, right_encoder_val);
    if (left_encoder_val2 <= 16)
    {
      md.setSpeeds(-offset * left_speed, offset * (right_speed + output));
      offset += 0.1;
    }
    else if (actual_distance - left_encoder_val2 <= 16)
    {
      md.setSpeeds(-offset * left_speed, offset * (right_speed + output));
      offset -= 0.1;
    }
    else
    {
      md.setSpeeds(-left_speed, right_speed + output);
    }
    if (left_encoder_val2 > actual_distance)
    {
      break;
    }

  }
  md.setBrakes(400, 400);
  resetPID();
}


float pidControlForward(int left_encoder_val, int right_encoder_val) {
  derivative = error - prevError;
  output = PID_KP * error + PID_KI * integral + PID_KD * derivative;
  prevError = error;


  return output;
}


float pidControlRight(int left_encoder_val, int right_encoder_val) {
  derivative = error - prevError;
  output = PID_KP_R * error + PID_KI_R * integral + PID_KD_R * derivative;
  prevError = error;


  return output;
}
