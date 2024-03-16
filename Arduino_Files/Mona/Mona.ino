#include <MsTimer2.h>

// motor direction mappings
#define FORWARD 0
#define BACKWARD 1

// Variables for 5 IR proximity sensors
int irRight, irRightFront, irFront, irLeftFront, irLeft;

// number of times to poll IR sensors over period of time
const int NUM_OF_READINGS = 5;

// pin mappings
const int IR_ENABLE = 4;
const int TOP_LED = 13;
const int LEFT_MOTOR_DIRECTION = 5;
const int RIGHT_MOTOR_DIRECTION = 6;
const int LEFT_MOTOR_SPEED = 10;
const int RIGHT_MOTOR_SPEED = 9;

// speed constants
const int LEFT_SPEED = 111;
const int RIGHT_SPEED = 111;

const int IR_THRESHOLD = 975;

const int LOOP_DELAY = 100;

// moving state of Mona tobot
enum State {
  movingLeft,
  movingRight,
  none
};
State state = none;

// TODO: delete later
// int counter = 0;

void irRead() {
  // IR enable
  digitalWrite(IR_ENABLE, HIGH);
  irRight = 0;
  irRightFront = 0;
  irFront = 0;
  irLeftFront = 0;
  irLeft = 0;

  // take NUM_OF_READINGS IR readings over a period of time and then average them
  for (int i = 0; i < NUM_OF_READINGS; i++) {
    irRight += analogRead(A3);
    irRightFront += analogRead(A2);
    irFront += analogRead(A1);
    irLeftFront += analogRead(A0);
    irLeft += analogRead(A7);
    delay(20);
  }

  irRight /= NUM_OF_READINGS;
  irRightFront /= NUM_OF_READINGS;
  irFront /= NUM_OF_READINGS;
  irLeftFront /= NUM_OF_READINGS;
  irLeft /= NUM_OF_READINGS;
}

void forward() {
  digitalWrite(LEFT_MOTOR_DIRECTION, FORWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, FORWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
}

void backward() {
  digitalWrite(LEFT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
}

void left() {
  digitalWrite(LEFT_MOTOR_DIRECTION, FORWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
}

void right() {
  digitalWrite(LEFT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, FORWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
}

void stop() {
  // need to set direction to forwards for some reason
  digitalWrite(LEFT_MOTOR_DIRECTION, FORWARD);
  analogWrite(LEFT_MOTOR_SPEED, 0);
  digitalWrite(RIGHT_MOTOR_DIRECTION, FORWARD);
  analogWrite(RIGHT_MOTOR_SPEED, 0);
}

void setup() {
  Serial.begin(9600);

  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  pinMode(IR_ENABLE, OUTPUT);
  pinMode(TOP_LED, OUTPUT);

  // initialise movement to forwards
  forward();
}

// void temp() {
//   if (counter < 2) {
//     forward();
//   } else if (counter < 4) {
//     left();
//   } else if (counter < 6) {
//     backward();
//   } else if (counter < 8) {
//     right();
//   } else if (counter < 10) {
//     stop();
//   } else {
//     counter = -1;
//   }
//   counter++;
// }

void move() {
  // if the robot is already moving left or right, then finish moving left or right 90 degrees
  if (state == movingLeft) {
    left();
  } else if (state == movingRight) {
    right();
  } else {
    // move left if possible
    // otherwise, move forward if possible
    // otherwise, move right
    if (irLeft > IR_THRESHOLD) {
      state = movingLeft;
      left();
    } else if (irFront > IR_THRESHOLD) {
      forward();
    } else {
      state = movingRight;
      right();
    }
  }
}

void loop() {
  digitalWrite(TOP_LED, HIGH);
  irRead();
  // temp();
  forward();
  digitalWrite(TOP_LED, LOW);
  delay(LOOP_DELAY);
}
