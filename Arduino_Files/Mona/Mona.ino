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

const int IR_FRONT_REVERSE_THRESHOLD = 995;
const int IR_LEFT_REVERSE_THRESHOLD = 985;
const int IR_RIGHT_REVERSE_THRESHOLD = 985;
const int IR_FRONT_THRESHOLD = 975;
const int IR_FRONT_SIDE_THRESHOLD = 975;
const int IR_THRESHOLD = 1000;

const int LOOP_DELAY = 100;

const int MOVE_FORWARD_DELAY = 300;
const int TURN_LEFT_DELAY = 100;
const int TURN_RIGHT_DELAY = 100;
const int REVERSE_DELAY = 600;

enum State {
  turningLeft,
  turningRight,
  normal
};
// whether Mona robot is currently partway through turning or not
State state = normal;

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
    irLeft += analogRead(A3);
    irLeftFront += analogRead(A2);
    irFront += analogRead(A1);
    irRightFront += analogRead(A0);
    irRight += analogRead(A7);
    delay(20);
  }

  irRight /= NUM_OF_READINGS;
  irRightFront /= NUM_OF_READINGS;
  irFront /= NUM_OF_READINGS;
  irLeftFront /= NUM_OF_READINGS;
  irLeft /= NUM_OF_READINGS;
}

void forward() {
  digitalWrite(LEFT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
}

void backward() {
  digitalWrite(LEFT_MOTOR_DIRECTION, FORWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, FORWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
}

void left() {
  digitalWrite(LEFT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, FORWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
}

void right() {
  digitalWrite(LEFT_MOTOR_DIRECTION, FORWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
}

void stop() {
  // need to set direction to forwards for some reason
  digitalWrite(LEFT_MOTOR_DIRECTION, FORWARD);
  analogWrite(LEFT_MOTOR_SPEED, 0);
  digitalWrite(RIGHT_MOTOR_DIRECTION, FORWARD);
  analogWrite(RIGHT_MOTOR_SPEED, 0);
}

void move() {
  // if the robot is already moving left, then finish moving left
  if (state == turningLeft) {
    left();
    delay(TURN_LEFT_DELAY);
    if (irFront > IR_FRONT_THRESHOLD) {
      state = normal;
    }
  } else if (state == turningRight) {
    right();
    delay(TURN_RIGHT_DELAY);
    if (irFront > IR_FRONT_THRESHOLD) {
      state = normal;
    }
  } else {
    if ((irFront < IR_FRONT_REVERSE_THRESHOLD) && (irLeft < IR_LEFT_REVERSE_THRESHOLD) && (irRight < IR_RIGHT_REVERSE_THRESHOLD)) {
      left();
      delay(REVERSE_DELAY);
      state = turningLeft;
    } else if (irLeftFront < IR_FRONT_SIDE_THRESHOLD) {
      right();
    } else if (irRightFront < IR_FRONT_SIDE_THRESHOLD) {
      left();
    } else if (irLeft > IR_THRESHOLD) {
      forward();
      delay(MOVE_FORWARD_DELAY);
      left();
      state = turningLeft;
    } else if (irFront < IR_FRONT_THRESHOLD) {
      right();
      state = turningRight;
    } else {
      forward();
    }
  }
}

// for debugging purposes
void printInfo() {
  // Serial.println("----------------------");
  // Serial.print("Left IR reading: ");
  // Serial.println(irLeft);
  // Serial.print("Front Left IR reading: ");
  // Serial.println(irLeftFront);
  // Serial.print("Front IR reading: ");
  // Serial.println(irFront);
  // Serial.print("Front Right IR reading: ");
  // Serial.println(irRightFront);
  // Serial.print("Right IR reading: ");
  // Serial.println(irRight);
  Serial.print("State: ");
  Serial.println(state);
  // Serial.println("----------------------");
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

void loop() {
  digitalWrite(TOP_LED, HIGH);
  irRead();
  move();
  printInfo();
  digitalWrite(TOP_LED, LOW);
  delay(LOOP_DELAY);
}
