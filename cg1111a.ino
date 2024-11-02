/* Import libraries */
#include "MeMCore.h"
#include "InterpolationLib.h"

/* Define constants */
#define TURNING_TIME_MS 330 // The time to turn 90 degrees (Need to be tuned)
#define TURN_CORRECTION_TIME_MS 50
#define STRAIGHT_RIGHT_TIME_MS 975
#define STRAIGHT_LEFT_TIME_MS 1050
#define ULTRASONIC 12
#define LED 13
#define SPEED_OF_SOUND 340
#define TIMEOUT 2000
#define LDRWait 30
#define IRWait 30
#define RGBWait 500

/* Initialisation of color-sensing constants */
const char R = 0, G = 1, B = 2;

/* Setup ports */
MeLineFollower lineFinder(PORT_2);
MeDCMotor leftMotor(M1);
MeDCMotor rightMotor(M2);
MeBuzzer buzzer;
MeRGBLed RGBled(0, 30);
MePort ir_adapter(PORT_3);
MePort ldr_adapter(PORT_4);

/* Define variables necessary for mBot */
int status = 0;
uint8_t motorSpeed = 180;
float colourArray[] = { 0, 0, 0 };
float whiteArray[] = { 0, 0, 0};
float blackArray[] = { 655.3, 953.7, 582.3 };
float greyDiff[] = { 279, 52.5, 348.7 };
// Interpolation Table for IR
const int numValues = 9;
double xValues[10] = { 720, 685, 655, 620, 590, 580, 550, 537, 500 }; // Analog output for IR
double yValues[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }; // Distance in cm
enum Color {  
  C_BLUE, C_GREEN, C_RED, C_ORANGE, C_PINK, C_WHITE
};
enum Motion {
  TWO_RIGHT, TURN_RIGHT, TURN_LEFT, U_TURN, TWO_LEFT, CHALLENGE, FORWARD, FINISH
};
Motion global_state = FORWARD;  // Default state of motion is FORWARD

/* Setup function */
void setup()
{
  pinMode(A7, INPUT);//button
  Serial.begin(9600);
  /* Calibrate Color Sensor */
  // setBalance();
  // for(int i=0;i<3;i+=1){
  //   Serial.print(greyDiff[i]);
  //   Serial.print(" ");
  //   Serial.println(blackArray[i]);
  // }
  // Setup mBot LED
  RGBled.setpin(LED);
}

/* Navigation functions */
/* Default motion */
void move_forward_correction(int correction) {
  leftMotor.run(-motorSpeed - correction);
  rightMotor.run(motorSpeed - correction);
}

void forward(int speed,int time){//more forward
  leftMotor.run(-speed);
  rightMotor.run(speed);
  delay(time); // Keep going straight for 500ms = 0.5s
  stop();
}

void stop(){
  leftMotor.stop();
  rightMotor.stop();
}

void turn(int side,int angle){//0--turn left, 1--turn right
  if (side == 0) {
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

/* Specific turn actions based on color detected */
void turn_left_time() {
  // Move forward slightly before turning left to align closer to the wall
  forward(motorSpeed, TURN_CORRECTION_TIME_MS);
  // delay(TURN_CORRECTION_TIME_MS);  
  
  turn_deg(0, 90);
  //delay(duration);

  stop();
  global_state = FORWARD;
}

void turn_right_time() {
  // Move forward slightly before turning right to align closer to the wall
  forward(motorSpeed, TURN_CORRECTION_TIME_MS);
  // delay(TURN_CORRECTION_TIME_MS);  

  turn_deg(1, 90);
  //delay(duration);

  // stop();
  global_state = FORWARD;
}

void uturn_time() {
  turn_deg(0, 180);
  // delay(duration);

  // stop();
  global_state = FORWARD;
}

void compound_turn_left() {
  // First turn
  turn_deg(0, 90);
  // delay(TWO_LEFT_TURN_TIME_MS); // purposely tuned to under-turn when turning left for compound to align to more closely to the wall on side of ultrasonic 

  // Move forward
  // stop();
  forward(motorSpeed, STRAIGHT_LEFT_TIME_MS);
  // delay(STRAIGHT_LEFT_TIME_MS);
  // stop();

  // Second turn
  turn_deg(0, 90);
  // delay(TWO_LEFT_TURN_TIME_MS);

  // stop();
  global_state = FORWARD;
}

void compound_turn_right() {
  // First turn
  turn_deg(1, 90);
  // delay(TWO_RIGHT_TURN_TIME_MS); // purposely tuned to under-turn when turning right for compound to align to more closely to the wall on side of ultrasonic

  // Move forward
  // stop();
  forward(motorSpeed, STRAIGHT_RIGHT_TIME_MS);
  // delay(STRAIGHT_RIGHT_TIME_MS);
  // stop();
  
  // Second turn
  turn_deg(1, 90);
  // delay(TWO_RIGHT_TURN_TIME_MS);

  // stop();
  global_state = FORWARD;
}

/* Ultrasonic Sensor */
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

int within_range() {
  double distance = left_distance();
  if (distance < 0.0 || distance > 15.0) {
    // No wall
    return 0;
  }
  if (distance < 7) {
    // Too close
    return -10;
  }
  else if (distance > 13) {
    // Too far
    return 10;
  }
  // Within Range
  return 0;
}

/* Color Sensor - Setup */
void Balance(int type){
  // 0: For White, 1: Black
  // Turn ON RED LED
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  if (type == 0) {
    whiteArray[0] = getAvgReadingLDR(5);
  } else if (type == 1) {
    blackArray[0] = getAvgReadingLDR(5);
  }
  
  // Turn ON GREEN LED
  ldr_adapter.dWrite1(LOW);
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  if (type == 0) {
    whiteArray[1] = getAvgReadingLDR(5);
  } else if (type == 1) {
    blackArray[1] = getAvgReadingLDR(5);
  }

  // Turn ON BLUE LED
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(LOW);
  delay(RGBWait);
  if (type == 0) {
    whiteArray[2] = getAvgReadingLDR(5);
  } else if(type == 1) {
    blackArray[2] = getAvgReadingLDR(5);
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

/* Color Sensor - Read */
void readColor(){
  // Turn ON RED LED
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  colourArray[0] = getAvgReadingLDR(5);
  
  // Turn ON GREEN LED
  ldr_adapter.dWrite1(LOW);//Green
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  colourArray[1] = getAvgReadingLDR(5);

  // Turn ON BLUE LED
  ldr_adapter.dWrite1(HIGH);//Blue
  ldr_adapter.dWrite2(LOW);
  delay(RGBWait);
  colourArray[2] = getAvgReadingLDR(5);

  delay(RGBWait);
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

int colour(){
  // 0: Blue, 1: Green, 2: Red, 3: Orange, 4: Pink
  readColor();
  delay(RGBWait);
  // Map the color measured within range
  for (int c = 0; c <= 2; c++) {
    colourArray[c] = (colourArray[c] - blackArray[c]) / greyDiff[c] * 255;
  }
  // Detect color
  int maxColor = maxx();
  if (maxColor == 2) {
    return C_BLUE;
  } else if(maxColor == 1) {
    return C_GREEN;
  } else {
    if (colourArray[1] < 130 && colourArray[2] < 130) {
      return C_RED;
    } else if (colourArray[1] < 200 || colourArray[2] < 200) {
      return C_ORANGE;
    } else {
      return C_PINK;
    }
  }
}

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

/* IR Sensor */
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

/* Buzzer */
void celebrate(){
  buzzer.tone(392, 200);
  buzzer.tone(523, 200);
  buzzer.tone(659, 200);
  buzzer.tone(784, 200);
  buzzer.tone(659, 150);
  buzzer.tone(784, 400);
  buzzer.noTone();
}

/* mBot LED */
void display_color(int c) {
  int ledColors[6][3] = {
    {0, 0, 255},    // BLUE
    {0, 255, 0},    // GREEN
    {255, 0, 0},    // RED
    {255, 100, 0},  // ORANGE
    {200, 0, 200},  // PINK
    {255, 255, 255} // WHITE
  };

  RGBled.setColor(ledColors[c][R], ledColors[c][G], ledColors[c][B]);
  RGBled.show();  // Show colours on-board arduino
}

/* mBot Line Sensor */
bool has_reached_waypoint() {
  /* Check if mBot line sensor has detected black line on the floor */
  int sensor_state = lineFinder.readSensors();
  return sensor_state == S1_IN_S2_IN;
}

/* Main function */
void loop()
{
  if (analogRead(A7) < 100) { //Press button
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
      // int col = identifyColour();
      if (col == C_RED) {
        turn(0,90);
      } else if (col == C_GREEN) {
        turn(1,90);
      } else if (col == C_ORANGE) {
        turn(1,180);
      } else if (col == C_PINK) {
        turn(0,93);
        forward(motorSpeed,650);
        delay(500);
        turn(0,93);
      } else if (col == C_WHITE) {
        celebrate();
      } else {
        // Blue
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

void loops()
{
  if (analogRead(A7) < 100) {
    status = 1 - status;
    delay(500);
  }
  if (status == 1){
    if (global_state == FORWARD) {
      int correction = within_range();
      move_forward_correction(correction);

      if (has_reached_waypoint()) {
        stop();
        global_state = CHALLENGE;
      }
    } else if (global_state = CHALLENGE) {
      int predicted_color = colour();
      display_color(predicted_color);

      if (predicted_color == C_WHITE) {
        stop();
        celebrate();
        global_state = FINISH; // Termintaes program
      } else {
        global_state = static_cast<Motion>(predicted_color);
      }
    } else if (global_state == TURN_LEFT) 
      turn_left_time();

    else if (global_state == TURN_RIGHT) 
      turn_right_time();
    
    else if (global_state == U_TURN)
      uturn_time();  // Turn LEFT for U-turn

    else if (global_state == TWO_LEFT) 
      compound_turn_left();
      
    else if (global_state == TWO_RIGHT) 
      compound_turn_right();

    delay(10);
  }
}
