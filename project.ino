/* Import libraries */
#include "MeMCore.h"
#include "InterpolationLib.h"

/* Hard-coded values for navigation */
#define TURNING_TIME_MS 325 //Maze 2: 337
#define STRAIGHT_RIGHT_TIME_MS 750
#define STRAIGHT_LEFT_TIME_MS 750
#define TWO_LEFT_TURN_TIME_MS 150
#define TWO_RIGHT_TURN_TIME_MS 150
#define RIGHT_DEVIATION 0
#define LEFT_DEVIATION 0
#define CORRECTION_SPEED 60
#define IR_TOO_CLOSE 600

#define LED 13
int status = 0;

/* Initialisation of mBot ports */
MeLineFollower lineFinder(PORT_2);
MeDCMotor leftMotor(M1);
MeDCMotor rightMotor(M2);
MeBuzzer buzzer;
MeRGBLed RGBled(0, 30);
MePort ir_adapter(PORT_3);
MePort ldr_adapter(PORT_4);

/* Initialisation of color-sensing constants */
const char R = 0, G = 1, B = 2;
float colourArray[] = { 0, 0, 0 }; 
float whiteArray[] = { 0, 0, 0};
float blackArray[] = { 709, 854, 730 };
float greyDiff[] = { 229, 135, 215 };

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
  // Setup mBot LED
  RGBled.setpin(LED);
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
      if (correction == 0) move_forward();
      else move_forward_correction(correction);
      display_color(C_WHITE);
      if (has_reached_waypoint()) {
        stop();
        global_state = CHALLENGE;
      }
    } else if (global_state == CHALLENGE) {
      readColor();
      int predicted_color = colour();
      display_color(predicted_color);

      if (predicted_color == C_WHITE) {
        stop();
        celebrate();
        global_state = FINISH; // Termintaes program
      } else {
        global_state = static_cast<Motion>(predicted_color);
      }
    } 
    else if (global_state == TURN_LEFT) turn_left_time();
    else if (global_state == TURN_RIGHT) turn_right_time();
    else if (global_state == U_TURN) uturn_time();
    else if (global_state == TWO_LEFT) compound_turn_left();
    else if (global_state == TWO_RIGHT) compound_turn_right();
    delay(10);
  }
}