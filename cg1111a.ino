/* Import libraries */
#include "MeMCore.h"
#include "InterpolationLib.h"

/* Define constants */
#define TURNING_TIME_MS 330 // The time to turn 90 degrees (Need to be tuned)
#define ULTRASONIC 12
#define LED 13
#define SPEED_OF_SOUND 340
#define TIMEOUT 2000
#define LDRWait 30
#define IRWait 30
#define RGBWait 500

/* Setup ports */
MeLineFollower lineFinder(PORT_2);
MeDCMotor leftMotor(M1);
MeDCMotor rightMotor(M2);
MeBuzzer buzzer;
MeRGBLed led(0, 30);
MePort ir_adapter(PORT_3);
MePort ldr_adapter(PORT_4);

/* Define variables necessary for mBot */
int status = 0;
uint8_t motorSpeed = 255; // Setting motor speed to an integer between 1 and 255, the larger the number, the faster the speed
float colourArray[] = { 0, 0, 0};
float blackArray[] = { 655.3, 953.7, 582.3 };
float greyDiff[] = { 279, 52.5, 348.7 };
// Interpolation Table for IR
const int numValues = 9;
double xValues[10] = { 720, 685, 655, 620, 590, 580, 550, 537, 500 }; // Analog output for IR
double yValues[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }; // Distance in cm

/* Setup function */
void setup()
{
  pinMode(A7, INPUT);//bottom
  Serial.begin(9600);
  // // setBalance();
  // for(int i=0;i<3;i+=1){
  //   Serial.print(greyDiff[i]);
  //   Serial.print(" ");
  //   Serial.println(blackArray[i]);
  // }
  // MBot LED
  led.setpin(LED);
}

/* Useful functions */
void stop(){
  leftMotor.stop();
  rightMotor.stop();
}

void forward(int speed,int time){//more forward
  leftMotor.run(-speed);
  rightMotor.run(speed);
  delay(time); // Keep going straight for 500ms = 0.5s
  stop();
}

void turn(int side,int angle){//0--turn left, 1--turn right
  if (side==0) {
    leftMotor.run(motorSpeed);
    rightMotor.run(motorSpeed);
    delay(angle * 3.2);
    stop();
  } else {
    leftMotor.run(-motorSpeed);
    rightMotor.run(-motorSpeed);
    delay(angle * 3.2);
    stop();
  }
}

void turn_deg(int side, int angle) {
  // TODO: Tune motorSpeed and the TURNING_TIME_MS to turn accurate an angle
  // 0: Turn Left, 1: Turn Right
  if (side == 0) {
    leftMotor.run(motorSpeed);
    rightMotor.run(motorSpeed);
    delay((double)angle / 90.0 * TURNING_TIME_MS);
    stop();
  } else {
    leftMotor.run(-motorSpeed);
    rightMotor.run(-motorSpeed);
    delay((double)angle / 90.0 * TURNING_TIME_MS);
    stop();
  }
}

double left_distance(){//ultrasonic sensor
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

void Balance(int type){//read colour
  ldr_adapter.dWrite1(HIGH);//RED
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  // if(type==0) whiteArray[0] = getAvgReadingLDR(5);
  // else if(type==1) blackArray[0] = getAvgReadingLDR(5);
  colourArray[0] = getAvgReadingLDR(5);
  
  ldr_adapter.dWrite1(LOW);//Green
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  // if(type==0) whiteArray[1] = getAvgReadingLDR(5);
  // else if(type==1) blackArray[1] = getAvgReadingLDR(5);
  colourArray[1] = getAvgReadingLDR(5);

  ldr_adapter.dWrite1(HIGH);//Blue
  ldr_adapter.dWrite2(LOW);
  delay(RGBWait);
  // if(type==0) whiteArray[2] = getAvgReadingLDR(5);
  //   else if(type==1) blackArray[2] = getAvgReadingLDR(5);
  colourArray[2] = getAvgReadingLDR(5);
  delay(RGBWait);
}

/*
void setBalance(){
  Serial.println("Put White Sample For Calibration ...");
  delay(5000);
  Balance(0);
  Serial.println("Put Black Sample For Calibration ...");
  delay(5000);
  Balance(1);
  for(int i = 0;i<=2;i++) greyDiff[i] = whiteArray[i] - blackArray[i];
  Serial.println("Colour Sensor Is Ready.");
  delay(1000);
}
*/

int getAvgReadingLDR(int times){      
  int reading;
  int total =0;
  for (int i = 0; i < times; i++){
    reading = ir_adapter.aRead2();
    total = reading + total;
    delay(LDRWait);
  }
  return total / times;
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

int maxx(){//find the three colors with the highest proportion
  int max=0;
  for (int c = 1; c <= 2; c++){
    if (colourArray[c] > colourArray[max]){
      max=c;
    }
  }
  return max;
}

int colour(){//0--Blue 1--Green 2--Red 3--Orange 4--Pink
  Balance(2);
  delay(500);
  for (int c = 0; c <= 2; c++) {
    colourArray[c]=(colourArray[c]-blackArray[c])/greyDiff[c]*255;
  }
  if (maxx() == 2) {
    return 0;
  } else if(maxx() == 1) {
    return 1;
  } else {
    if (colourArray[1] < 130 && colourArray[2] < 130) {
      return 2;
    } else if (colourArray[1] < 200 || colourArray[2] < 200) {
      return 3;
    } else {
      return 4;
    }
  }
}

// Play the celebration song when reaching the end
void celebrate(){
  buzzer.tone(392, 200);
  buzzer.tone(523, 200);
  buzzer.tone(659, 200);
  buzzer.tone(784, 200);
  buzzer.tone(659, 150);
  buzzer.tone(784, 400);
  buzzer.noTone();
}

// Blink RED LED on mBot
void blinkRed(){
  led.setColor(0, 255, 0, 0);
  led.setColor(1, 255, 0, 0);
  led.show();
  delay(500);
}

// Blink GREEN LED on mBot
void blinkGreen(){
  led.setColor(0, 0, 255, 0);
  led.setColor(1, 0, 255, 0);
  led.show();
  delay(500);
}

// Blink ORANGE LED on mBot
void blinkOrange(){
  led.setColor(0, 255, 165, 0);
  led.setColor(1, 255, 165, 0);
  led.show();
  delay(500);
}

// Blink PINK LED on mBot
void blinkPink() {
  led.setColor(0, 255, 192, 203);
  led.setColor(1, 255, 192, 203);
  led.show();
  delay(500);
}

// Blink BLUE LED on mBot
void blinkBlue(){
  led.setColor(0, 0, 0, 255);
  led.setColor(1, 0, 0, 255);
  led.show();
  delay(500);
}

double getDistanceIR(float x)
{
  // The last boolean flag "clamp", true will limit the y output in the range of the known values.
  double distance = Interpolation::Linear(xValues, yValues, numValues, x, false);
  return distance;
}

/* Main function */
void loop()
{
  if (analogRead(A7) < 100) {
    status = 1 - status;
    delay(500);
  }
  if (status==1){
    /*int col = colour();
    Serial.println(col);
    delay(1000);*/
    double left_dis = left_distance();
    if (left_dis < 7) {
      turn(1, 10); //if leftdistance smaller than 10cm, turn right 10 degrees
    } else if(left_dis>13) { 
      turn(0, 10);
    }
    int sensorState = lineFinder.readSensors();
    Serial.println(sensorState);
    if (sensorState != 3) {//if detect black, stop and detect colour
      stop();
      int col = colour();
      if (col == 2) {
        turn(0,90);
      } else if (col == 1) {
        turn(1,90);
      } else if (col == 3) {
        turn(1,180);
      } else if (col == 4) {
        turn(0,93);
        forward(motorSpeed,650);
        delay(500);
        turn(0,93);
      } else {
        turn(1,93);
        forward(motorSpeed,650);
        delay(500);
        turn(1,93);
      }
      delay(100);
      forward(motorSpeed,1000);
    }
    forward(motorSpeed,100);
  }
  /*Serial.println("put colout");
  delay(3000);
  int col=colour();
  Serial.println(col);
  delay(1000);*/

  // IR Reading
  // int ir_reading = getAvgReadingIR(5);
  // double distance = getDistanceIR((float)ir_reading);
  // Serial.print("Distance: ");
  // Serial.println(distance);
  // delay(1000);
}