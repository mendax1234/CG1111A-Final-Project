#include "MeMCore.h"
#include "InterpolationLib.h"

/* Define constants */
#define TURNING_TIME_MS 417 // The time to turn 90 degrees (Need to be tuned)
#define TURN_CORRECTION_TIME_MS 50
#define STRAIGHT_RIGHT_TIME_MS 985
#define STRAIGHT_LEFT_TIME_MS 980
#define ULTRASONIC 12
#define LED 13
#define SPEED_OF_SOUND 340
#define TIMEOUT 2000
#define LDRWait 30
#define RGBWait 500
MeLineFollower lineFinder(PORT_2);
MeDCMotor leftMotor(M1);
MeDCMotor rightMotor(M2);
MeBuzzer buzzer;
MeRGBLed led(0, 30);
int status = 0;
uint8_t motorSpeed = 255; // Setting motor speed to an integer between 1 and 255, the larger the number, the faster the speed
MePort ir_adapter(PORT_3);
MePort ldr_adapter(PORT_4);
float colourArray[] = {0,0,0};
float blackArray[] = {655.3,953.7,582.3};
float greyDiff[] = {279,52.5,348.7};

void setup()
{
  pinMode(A7, INPUT);//bottom
  Serial.begin(9600);
  // setBalance();
  // for(int i=0;i<3;i+=1){
  //   Serial.print(greyDiff[i]);
  //   Serial.print(" ");
  //   Serial.println(blackArray[i]);
  // }
  // MBot LED
  led.setpin(LED);
}

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
  if(side==0){
    leftMotor.run(motorSpeed);
    rightMotor.run(motorSpeed);
    delay(angle * 3.2);
    stop();
  }else{
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
  //forward(motorSpeed, TURN_CORRECTION_TIME_MS);
  // delay(TURN_CORRECTION_TIME_MS);  
  
  turn_deg(0, 90);
  //delay(duration);

  stop();
  global_state = FORWARD;
}

void turn_right_time() {
  // Move forward slightly before turning right to align closer to the wall
  // forward(motorSpeed, TURN_CORRECTION_TIME_MS);
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
  delay(RGBWait);

  // delay(TWO_LEFT_TURN_TIME_MS); // purposely tuned to under-turn when turning left for compound to align to more closely to the wall on side of ultrasonic 

  // Move forward
  // stop();
  forward(motorSpeed, STRAIGHT_LEFT_TIME_MS);
  delay(RGBWait);
  // delay(STRAIGHT_LEFT_TIME_MS);
  // stop();

  // Second turn
  turn_deg(0, 95);
  // delay(TWO_LEFT_TURN_TIME_MS);

  // stop();
  global_state = FORWARD;
}

void compound_turn_right() {
  // First turn
  turn_deg(1, 90);
  delay(RGBWait);
  // delay(TWO_RIGHT_TURN_TIME_MS); // purposely tuned to under-turn when turning right for compound to align to more closely to the wall on side of ultrasonic

  // Move forward
  // stop();
  forward(motorSpeed, STRAIGHT_RIGHT_TIME_MS);
  delay(RGBWait);
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
  return duration / 2.0 / 1000000 * SPEED_OF_SOUND * 100; // unit is cm
}

void Balance(int type){//read colour
  ldr_adapter.dWrite1(HIGH);//RED
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  // if(type==0) whiteArray[0] = getAvgReading(5);
  // else if(type==1) blackArray[0] = getAvgReading(5);
  colourArray[0] = getAvgReading(5);
  
  ldr_adapter.dWrite1(LOW);//Green
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  // if(type==0) whiteArray[1] = getAvgReading(5);
  // else if(type==1) blackArray[1] = getAvgReading(5);
  colourArray[1] = getAvgReading(5);

  ldr_adapter.dWrite1(HIGH);//Blue
  ldr_adapter.dWrite2(LOW);
  delay(RGBWait);
  // if(type==0) whiteArray[2] = getAvgReading(5);
  //   else if(type==1) blackArray[2] = getAvgReading(5);
  colourArray[2] = getAvgReading(5);
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
int getAvgReading(int times){      
  int reading;
  int total =0;
  for(int i = 0;i < times;i++){
     reading = ir_adapter.aRead2();
     total = reading + total;
     delay(LDRWait);
  }
  return total/times;
}

int maxx(){//find the three colors with the highest proportion
  int max=0;
  for(int c=1;c<=2;c++){
    if(colourArray[c]>colourArray[max]){
      max=c;
    }
  }
  return max;
}

int colour(){//0--Blue 1--Green 2--Red 3--Orange 4--Pink
  Balance(2);
  delay(500);
  for(int c=0;c<=2;c++) colourArray[c]=(colourArray[c]-blackArray[c])/greyDiff[c]*255;
  if(maxx()==2) return 0;
  else if(maxx()==1) return 1;
  else{
    if(colourArray[1]<130&&colourArray[2]<130) return 2;
    else if(colourArray[1]<200||colourArray[2]<200) return 3;
    else return 4;
  }
}

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
  Serial.print("Sensor State:");
  Serial.println(sensor_state);
  Serial.print("Target State:");
  Serial.println(S1_IN_S2_IN);
  return sensor_state == S1_IN_S2_IN;
}

/* Tune Constatnts */
void tune_constants(){
  /* Turn Left */
  //turn_deg(0, 90);
  //Serial.print("90 deg Time Constant: ");
  //Serial.println(TURNING_TIME_MS);
  /* Turn Right */
  //turn_deg(1, 90);
  //Serial.print("90 deg Time Constant: ");
  //Serial.println(TURNING_TIME_MS);
  //turn_left_time();
  //turn_right_time();
  //uturn_time();
  //compound_turn_left();
  //compound_turn_right();
}

/* Main function */
void loops()
{
  if (analogRead(A7) < 100) {
    status = 1 - status;
    delay(500);
  }
  if(status==1){
    /*int col = colour();
    Serial.println(col);
    delay(1000);*/
    double left_dis=left_distance();
    if(left_dis<7) turn(1,10);//if leftdistance smaller than 10cm, turn right 10 degrees
    else if(left_dis>13) turn(0,10);
    int sensorState = lineFinder.readSensors();
    Serial.println(sensorState);
    if(sensorState != 3){//if detect black, stop and detect colour
      stop();
      int col = colour();
      if(col == 2) turn(0,90);
      else if(col == 1) turn(1,90);
      else if(col == 3) turn(1,180);
      else if(col == 4){
        turn(0,93);
        forward(motorSpeed,650);
        delay(500);
        turn(0,93);
      }else{
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
  
  // Code to play celebrate sound
  // celebrate();
  // delay(1000);

  // Code to turn on MBot LED
  // blinkRed();
  // delay(1000);
  // blinkBlue();
  // delay(1000);
}

void loop()
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
    } else if (global_state == CHALLENGE) {
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
    //tune_constants();
    //delay(15000);

  } 
  
}
