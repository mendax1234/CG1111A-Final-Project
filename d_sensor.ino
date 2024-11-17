/*
  d_sensor.ino: take in values from sensors & output readings to be used for navigation & colour sensing
*/

/* Ultrasonic Sensor */
#define TIMEOUT 2000
#define SPEED_OF_SOUND 340
#define ULTRASONIC 12

double left_distance(){
  // Get distance away from the left board (Ultrasonic Sensor)
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC, LOW);
  pinMode(ULTRASONIC, INPUT);
  long duration = pulseIn(ULTRASONIC, HIGH, TIMEOUT);
  if (duration > 0) {
    return duration / 2.0 / 1000000 * SPEED_OF_SOUND * 100; // unit is cm
  }
  return -1;
}

/* Color Sensor - Setup */
#define RGBWait 100
#define LDRWait 30

void Balance(int type){
  // 0: For White, 1: Black
  // Turn ON RED LED
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  if (type == 0) {
    whiteArray[0] = getAvgReadingLDR(3);
  } else if (type == 1) {
    blackArray[0] = getAvgReadingLDR(3);
  }
  
  // Turn ON GREEN LED
  ldr_adapter.dWrite1(LOW);
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  if (type == 0) {
    whiteArray[1] = getAvgReadingLDR(3);
  } else if (type == 1) {
    blackArray[1] = getAvgReadingLDR(3);
  }

  // Turn ON BLUE LED
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(LOW);
  delay(RGBWait);
  if (type == 0) {
    whiteArray[2] = getAvgReadingLDR(3);
  } else if(type == 1) {
    blackArray[2] = getAvgReadingLDR(3);
  }
  delay(RGBWait);
}

void setBalance(){
  // Calibration: To get whiteArray, blackArray and greyDiff
  Serial.println("Put White Sample For Calibration ...");
  delay(5000);
  Balance(0);
  Serial.println("Put Black Sample For Calibration ...");
  delay(5000);
  Balance(1);
  for(int i = 0; i <= 2; i++) {
    greyDiff[i] = whiteArray[i] - blackArray[i];
  }
  Serial.println("Colour Sensor Is Ready.");
  delay(1000);
}

/* Color Sensor - Read*/
int getAvgReadingLDR(int times){      
  int reading;
  int total =0;
  for (int i = 0; i < times; i++){
    reading = ir_adapter.aRead1();
    total = reading + total;
    delay(LDRWait);
  }
  return total / times;
}

int maxx(){
  // Find the three colors with the highest proportion
  int max = 0;
  for (int c = 1; c <= 2; c++){
    if (colourArray[c] > colourArray[max]){
      max = c;
    }
  }
  return max;
}

void readColor(){
  // Turn ON RED LED
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  colourArray[0] = getAvgReadingLDR(3);
  
  // Turn ON GREEN LED
  ldr_adapter.dWrite1(LOW);//Green
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  colourArray[1] = getAvgReadingLDR(3);

  // Turn ON BLUE LED
  ldr_adapter.dWrite1(HIGH);//Blue
  ldr_adapter.dWrite2(LOW);
  delay(RGBWait);
  colourArray[2] = getAvgReadingLDR(3);

  delay(RGBWait);
}

/* IR Sensor */
#define IRWait 5
const int numValues = 9;
double xValues[10] = { 814, 791, 735, 696, 643, 564, 510, 473, 479 }; // Analog output for IR
double yValues[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }; // Distance in cm

int ir_read() {
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(LOW);
  delay(IRWait);

  int ambient = ir_adapter.aRead1();
  ldr_adapter.dWrite1(LOW);
  ldr_adapter.dWrite2(LOW);
  delay(IRWait);
  int reading = ir_adapter.aRead1();
  int difference = ambient - reading;
  return difference;
}

int getAvgReadingIR(int times){
  int reading;
  int total = 0;
  for (int i = 0; i < times; i++) {
    reading = ir_adapter.aRead1();
    total = reading + total;
    delay(IRWait);
  }
  return total / times;
}

double getDistanceIR(float x)
{
  // The last boolean flag "clamp", true will limit the y output in the range of the known values.
  double distance = Interpolation::Linear(xValues, yValues, numValues, x, false);
  return distance;
}

/* mBot Line Sensor */
bool has_reached_waypoint() {
  /* Check if mBot line sensor has detected black line on the floor */
  int sensor_state = lineFinder.readSensors();
  if (sensor_state != S1_OUT_S2_OUT)
  {
    if (sensor_state == S1_IN_S2_OUT)
    {
      // Turn left a bit
      turn_deg(0, 20);
    }
    else if (sensor_state == S1_OUT_S2_IN)
    {
      // Turn right a bit
      turn_deg(1, 20);
    }
    return true;
  }
  return false;
}
