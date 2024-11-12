/*
  b_navigation.ino: Motion for the motors, including auto line correction & complex turning algorithms to overcome compound turn and fringe left turn cases
*/

/* IR correction */
#define IRWait 30
int ir_read() {
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(LOW);
  delay(IRWait);

  int ambient = ir_adapter.aRead1();
  //  Serial.print("ambient: ");
  //  Serial.println(ambient);
  ldr_adapter.dWrite1(LOW);
  ldr_adapter.dWrite2(LOW);
  delay(IRWait);
  int reading = ir_adapter.aRead1();
  //  Serial.print("reading: ");
  //  Serial.println(reading);
   Serial.print("difference: ");
   Serial.println(ambient - reading);
  int difference = ambient - reading;
  return difference;
}

/* Motion Constants */
uint8_t motorSpeed = 220;

/* Default motion */
void move_forward_correction(int correction) {
  leftMotor.run(-motorSpeed + correction);
  rightMotor.run(motorSpeed + correction);
  Serial.println("Correction!");
}

void move_forward() {
  leftMotor.run(-motorSpeed - LEFT_DEVIATION);
  rightMotor.run(motorSpeed + RIGHT_DEVIATION);
}

void forward(int speed,int time){//more forward
  leftMotor.run(-speed - LEFT_DEVIATION);
  rightMotor.run(speed + RIGHT_DEVIATION);
  delay(time); // Keep going straight for 500ms = 0.5s
  stop();
}

void stop(){
  leftMotor.stop();
  rightMotor.stop();
}

void turn_deg(int side, int angle) {
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

/* Auto correction function */
int within_range() {
  double distance = left_distance();
  if (distance < 0.0 || distance > 15.0) {
    // No wall
    return 0;
  }
  if (distance < 10) {
    // Too close
    return -CORRECTION_SPEED;
  }
  if (distance > 13 || ir_diff > 510) {
    // Too far
    return CORRECTION_SPEED;
  }
  // Within Range
  return 0;
}

/* Specific turn actions based on color detected */
void turn_left_time() {
  turn_deg(0, 90);
  global_state = FORWARD;
}

void turn_right_time() {
  turn_deg(1, 90);
  global_state = FORWARD;
}

void uturn_time() {
  double distance = left_distance();
  if (distance > 0 && distance < 7)
  {
    // Too close to left
    turn_deg(1, 190);
  }
  turn_deg(0, 170);
  global_state = FORWARD;
}

void compound_turn_left() {
  // First turn
  turn_deg(0, 90);
  delay(TWO_LEFT_TURN_TIME_MS); // purposely tuned to under-turn when turning left for compound to align to more closely to the wall on side of ultrasonic 
  // Move forward
  forward(motorSpeed, STRAIGHT_LEFT_TIME_MS);
  delay(TWO_LEFT_TURN_TIME_MS);
  // Second turn
  turn_deg(0, 87);
  global_state = FORWARD;
}

void compound_turn_right() {
  // First turn
  turn_deg(1, 90);
  delay(TWO_RIGHT_TURN_TIME_MS); // purposely tuned to under-turn when turning right for compound to align to more closely to the wall on side of ultrasonic
  // Move forward
  forward(motorSpeed, STRAIGHT_RIGHT_TIME_MS);
  delay(TWO_RIGHT_TURN_TIME_MS);
  // Second turn
  turn_deg(1, 87);
  global_state = FORWARD;
}
