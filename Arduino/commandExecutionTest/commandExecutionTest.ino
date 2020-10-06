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

#define CALI_DISTANCE_OFFSET -0.5

#define ERROR_RIGHT 0.06
#define ERROR_FRONT 0

int RUN = 32;

const int maxCalibrationTrial = 30;
boolean calibration = true;

int caliReading[49]; //sensor readings for calibration


double Distance[29];
String gridsToRpi = "";


volatile int left_encoder_val = 0, right_encoder_val = 0, left_encoder_val2 = 0;

float prevError = 0.0;
float integral = 0.0;

float testSpeed = 320; //180                              //
float PID_KP = 1.6; // lower = right, higher = left     // computePID() //-1.3
float PID_KI = 0.0019;  //0.01                                       // computePID()
float PID_KD = 0; //0.05                                       // computePID()
float GRID_DISTANCE[5] = {10.15, 20.3, 29.7, 40.1, 51};   // moveByGrids(int)
float DG_GRID_DISTANCE[5] = {14.7, 20.5, 29.7, 40.1, 51}; // moveByDgGrids(int)
float LEFT_ROTATION_DEGREE_OFFSET = -3.5;                   // rotateLeft(int)
float RIGHT_ROTATION_DEGREE_OFFSET = -2.6;                  // rotateRight(int)
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
  
 
  
    //Serial.println(calculateGrids(RB));
 //aliAngleRight();
  /*
  float reading;
   for (int i = 0; i < 29; i++)
      {
        Distance[i] = ll.distance();
      }
      timSort(Distance, 29);

      reading =  Distance[29 / 2];
    Serial.println(reading);
   delay(200);
   */
}

void commandExecution(char cmd)
{
  switch (cmd) {
    case 'F':
      //md.setSpeeds(100,100);
      moveByGrids(1);
      delay(200);
      sendSensorReading();
      //Serial.println("k");
      break;
    case 'L':
      //md.setSpeeds(100, -100);
      rotateLeft(90);
      delay(200);
      sendSensorReading();
      //Serial.println("L ok");
      break;
    case 'R':
      //md.setSpeeds(-100, 100);
      rotateRight(90);
      delay(200);
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
      delay(100);
      caliDistanceFront();
      delay(100);
      caliAngleRight();
      delay(100);
      rotateRight(90);
      delay(200);
      caliAngleFront();
      delay(100);
      caliDistanceFront();
      delay(100);
      caliAngleFront();
      delay(100);
      rotateRight(90);
      delay(200);
      Serial.println("k");
      break;
    case 'X':
//      delay(100);
      caliDistanceFront();
      //delay(100);
      //sendSensorReading();
      Serial.println("k");
      break;
    case 'Y':
//      delay(100);
      caliAngleRight();
      //delay(100);
      //sendSensorReading();
      Serial.println("k");
      break;

  }
}


//================================================Send distance to RPI========================================================================

void sendSensorReading()
{
  for (int i = 0; i<5; i++)
  {
    gridsToRpi = gridsToRpi + "SDATA;1_" + calculateGrids(FL) + ";2_" + calculateGrids(FC) + ";3_" + calculateGrids(FR) + ";4_" + calculateGrids(RF) + ";5_" + calculateGrids(RB) + ";6_" + calculateGrids(LL) + ";";
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
      else if (reading >= 18 && reading <= 29)
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
      if (reading >= 7 && reading <= 17)
        return "1";
      else if (reading >= 18 && reading <= 21)
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
      if (reading >= 7 && reading <= 14)
        return "1";
      else if (reading >= 15 && reading <= 27)
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
      if (reading >= 17 && reading <= 24)
        return "1";
      else if (reading >= 25 && reading <= 33)
        return "2";
      else if (reading >= 34 && reading <= 43 )
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
      else if (reading >= 17 && reading <= 27)
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
      if (reading >= 7 && reading <= 21)
        return "1";
      else if (reading >= 22 && reading <= 34)
        return "2";
      else return "-1";
  }
}
void qselect(double A[], int start, int end, int k) {
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
void insertionSort(double arr[], int left, int right)
{
    for (int i = left + 1; i <= right; i++)
    {
        int temp = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > temp)
        {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = temp;
    }
}

void merge(double arr[], int l, int m, int r)
{
    // original array is broken in two parts
    // left and right array
    int len1 = m - l + 1, len2 = r - m;
    int left[len1], right[len2];
    for (int i = 0; i < len1; i++)
        left[i] = arr[l + i];
    for (int i = 0; i < len2; i++)
        right[i] = arr[m + 1 + i];

    int i = 0;
    int j = 0;
    int k = l;

    // after comparing, we merge those two array
    // in larger sub array
    while (i < len1 && j < len2)
    {
        if (left[i] <= right[j])
        {
            arr[k] = left[i];
            i++;
        }
        else
        {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    // copy remaining elements of left, if any
    while (i < len1)
    {
        arr[k] = left[i];
        k++;
        i++;
    }

    // copy remaining element of right, if any
    while (j < len2)
    {
        arr[k] = right[j];
        k++;
        j++;
    }
}

void timSort(double arr[], int n)
{
    // Sort individual subarrays of size RUN
    for (int i = 0; i < n; i+=RUN)
        insertionSort(arr, i, min((i+31), (n-1)));

    // start merging from size RUN (or 32). It will merge
    // to form size 64, then 128, 256 and so on ....
    for (int size = RUN; size < n; size = 2*size)
    {
        // pick starting point of left sub array. We
        // are going to merge arr[left..left+size-1]
        // and arr[left+size, left+2*size-1]
        // After every merge, we increase left by 2*size
        for (int left = 0; left < n; left += 2*size)
        {
            // find ending point of left sub array
            // mid+1 is starting point of right sub array
            int mid = left + size - 1;
            int right = min((left + 2*size - 1), (n-1));

            // merge sub array arr[left.....mid] &
            // arr[mid+1....right]
            merge(arr, left, mid, right);
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
 

  while (abs(error) > 0.1 && counter < 30)
  {
    //Serial.println(error);
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
    flDistance = sensorCaliDistance(FL);
    frDistance = sensorCaliDistance(FR);
    error = flDistance - frDistance;
    //Serial.println(error);
    counter++;

  }
}

void caliDistanceFront()
{
  float targetDistance = 5 +CALI_DISTANCE_OFFSET;
  float actualDistance = sensorCaliDistance(FL);
  float difference;
  int counter = 0;
  difference = targetDistance - actualDistance;
  while (abs(difference) > 0.075 && counter < 30)
  {
    if (difference > 0)
    {
      moveBackwardCali(); //move forward for 0.2cm
    }
    else
    {
      moveForwardCali(); //move backward for 0.2cm
    }
    delay(10);
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
    rfDistance = sensorCaliDistance(RF);
    rbDistance = sensorCaliDistance(RB);
    error = rfDistance - rbDistance;
    counter ++;
  }
  
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
      Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = (float) caliReading[49 / 2] / 1023 * 5;

      //Distance = -2.0927 * pow(voltage, 3) + 16.475 * voltage * voltage -47.533 * voltage + 52.481;
      Distance = -6.3295 * pow(voltage, 3) + 45.689 * voltage * voltage -113.84 * voltage + 101.02;
      //distance =
      return Distance;

    case FR:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(FR);
      }
      Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = (float) caliReading[49 / 2] / 1023 * 5;
      //Distance = -4.4405 * pow(voltage, 3) + 30.795 * voltage * voltage -76.011 * voltage + 70.041;
      Distance = -1.7829 * pow(voltage, 3) + 14.583 * voltage * voltage -43.971 * voltage + 49.982;
      //distance =
      return Distance;
    case RF:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(RF);
      }
      Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = (float) caliReading[49 / 2] / 1023 * 5;
      Distance = -1.7829 * pow(voltage, 3) + 14.583 * voltage * voltage + -43.971 * voltage + 49.982;
      //distance =
      return Distance;
    case RB:
      for (int i = 0; i < NUM_SENSOR_READINGS_CALI; i++)
      {
        caliReading[i] = analogRead(RB);
      }
      Sort(caliReading, NUM_SENSOR_READINGS_CALI);
      //add formula for distance calculation
      voltage = (float) caliReading[49 / 2] / 1023 * 5;
      Distance = -3.3307* pow(voltage, 3) + 24.122 * voltage * voltage -62.573 * voltage + 63.155; //doenst works when distance less than 2.5cm
      //distance =
      return Distance;

  }

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
