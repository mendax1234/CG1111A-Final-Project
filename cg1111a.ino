#include "MeMCore.h"
#define TURNING_TIME_MS 330
#define ULTRASONIC 12
#define SPEED_OF_SOUND 340
#define TIMEOUT 2000
#define LDRWait 30
#define RGBWait 500
MeLineFollower lineFinder(PORT_2);
MeDCMotor leftMotor(M1);
MeDCMotor rightMotor(M2);
int status = 0;
uint8_t motorSpeed = 255;
MePort ir_adapter(PORT_3);
MePort ldr_adapter(PORT_4);
int A = 1;
int B = 2;  
int ldrPin = A1;
float colourArray[] = {0,0,0};
float whiteArray[] = {0,0,0};
float blackArray[] = {0,0,0};
float greyDiff[] = {0,0,0};
void setup()
{
  pinMode(A7, INPUT);
  Serial.begin(9600);
  setBalance();
}
void stop(){
  leftMotor.stop();
  rightMotor.stop();
}
void forward(int speed){
  leftMotor.run(-speed);
  rightMotor.run(speed);
  delay(500);
  stop();
}
void turn(int side,int angle){
  if(side==0){
    leftMotor.run(motorSpeed);
    rightMotor.run(motorSpeed);
    delay(angle*3.2);
    stop();
  }else if(side==1){
    leftMotor.run(-motorSpeed);
    rightMotor.run(-motorSpeed);
    delay(angle*3.2);
    stop();
  }
}
double left_distance(){
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC, LOW);
  pinMode(ULTRASONIC, INPUT);
  long duration = pulseIn(ULTRASONIC, HIGH, TIMEOUT);
  return duration / 2.0 / 1000000 * SPEED_OF_SOUND * 100;
}
void Balance(int type){
  ldr_adapter.dWrite1(HIGH);//RED
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  if(type==0) whiteArray[0] = getAvgReading(5);
  else if(type==1) blackArray[0] = getAvgReading(5);
  else if(type==2) colourArray[0] = getAvgReading(5);
  
  ldr_adapter.dWrite1(LOW);//Green
  ldr_adapter.dWrite2(HIGH);
  delay(RGBWait);
  if(type==0) whiteArray[1] = getAvgReading(5);
  else if(type==1) blackArray[1] = getAvgReading(5);
  else if(type==2) colourArray[1] = getAvgReading(5);

  ldr_adapter.dWrite1(HIGH);//Blue
  ldr_adapter.dWrite2(LOW);
  delay(RGBWait);
  if(type==0) whiteArray[2] = getAvgReading(5);
    else if(type==1) blackArray[2] = getAvgReading(5);
  else if(type==2) colourArray[2] = getAvgReading(5);
  delay(RGBWait);
}
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
int maxx(){
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
  /*for(int c=0;c<=2;c++){
    Serial.print(colourArray[c]);
    Serial.print(" ");
  }
  Serial.println();*/
  if(maxx()==2) return 0;
  else if(maxx()==1) return 1;
  else{
    if(colourArray[1]<200||colourArray[2]<200) return 2;
    else if(colourArray[1]<230&&colourArray[2]<230) return 3;
    else return 4;
  }
}
void loop()
{
  /*if (analogRead(A7) < 100) {
    status = 1 - status;
    delay(500);
  }
  if(status==1){
    double left_dis=left_distance();
    if(left_distance<6){
     turn(1,20);
    }
    int sensorState = lineFinder.readSensors();
    if(sensorState == S1_IN_S2_IN){
      stop();
      delay(10000);
    }
    forward(motorSpeed);
  }*/
  Serial.println("put colout");
  delay(3000);
  int col=colour();
  Serial.println(col);
  delay(1000);
}
