#define FR A0 //front right sensor
#define FC A4 //front center sensor
#define FL A1 //front left sensor
#define LL A3 //left long range sensor
#define LS A5 //left short range sensor
#define R A2 //right sensor


#define NUMBER_OF_SENSOR_READINGS 49

void setup()
{
  Serial.begin(9600);
  
}

void loop()
{
   //float reading = getSensorReading(NUMBER_OF_SENSOR_READINGS, LL);
   //float distance = calDistance(reading, LL);
   //Serial.println(distance);
   float reading[49];
   float distance[49];
   for(int i = 0; i<49; i++)
   {
    reading[i] = getSensorReading(NUMBER_OF_SENSOR_READINGS,LL);
    distance[i] = calDistance(reading[i], LL);
   }

   insertionsortFloat(distance, 49);
   Serial.println(distance[49/2]);
   
   
}

float calDistance(int reading, int sensor)
{
  float distance;//in cm
  switch (sensor) {
    case FL:
      //distance = -24.04 + 1811796 / (1+pow(reading/0.0000015, 0.57));
      //distance = 18130*pow(reading, -1.319);
      //distance = 25523*pow(reading, -1.382);//10cm to 50cm
      distance = 9439.9*pow(reading, -1.182); //30cm to 50cm
      break;
    case FC:
      //distance = -3.24 + 88316817/(1+pow(reading/0.000021, 0.93));
      //distance = 8610.4*pow(reading, -1.094);
      //distance = 16721*pow(reading, -1.232); //30cm to 50cm
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
