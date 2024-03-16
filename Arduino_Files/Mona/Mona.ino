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

const int IR_THRESHOLD = 1000;

const int LOOP_DELAY = 100;

// ~2.65s for 1 full 360 degrees rotation (at 111 speed)
const int TIME_FOR_FULL_ROTATION = 1210;

// ~1.75s for moving forwards 1 full block (at 111 speed)
const int TIME_FOR_FULL_BLOCK = 1600;

enum State {
  turningLeft,
  turningRight,
  movingForward,
  normal
};
// whether Mona robot is currently partway through turning, moving or neither
State state = normal;

// 0 degrees is original orientation
// negative means left of original orientation
// positive means right of original orientation
double bearing = 0;
double wantedBearing = 0;

// counter for moving forwards 1 block
int counter = 0;

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
  if (state != normal) {
    handleState();
  } else {
    // move left if possible
    // otherwise, move forward if possible
    // otherwise, move right
    if (irLeft > IR_THRESHOLD) {
      state = turningLeft;
      wantedBearing = bearing - 90;
      handleState();
    } else if (irFront > IR_THRESHOLD) {
      state = movingForward;
      handleState();
    } else {
      state = turningRight;
      wantedBearing = bearing + 90;
      handleState();
    }
  }
}

void handleState() {
  if (state == turningLeft) {
    left();
    bearing -= 360.0 / TIME_FOR_FULL_ROTATION * LOOP_DELAY;
    // already turned left past wanted bearing
    Serial.print("Bearing: ");
    Serial.println(bearing);
    if (wantedBearing >= bearing) {
      state = movingForward;
      bearing = 0;
    }
  } else if (state == turningRight) {
    right();
    bearing += 360.0 / TIME_FOR_FULL_ROTATION * LOOP_DELAY;
    // already turned right past wanted bearing
    if (wantedBearing <= bearing) {
      state = movingForward;
      bearing = 0;
    }
  } else if (state == movingForward) {
    forward();
    counter += LOOP_DELAY;
    // already gone past 1 block
    if (counter >= TIME_FOR_FULL_BLOCK) {
      state = normal;
      counter = 0;
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
  // Serial.print("Bearing: ");
  // Serial.println(bearing);
  // Serial.print("Wanted bearing: ");
  // Serial.println(wantedBearing);
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
