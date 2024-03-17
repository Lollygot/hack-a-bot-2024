#include "MsTimer2.h"
#include "RF24.h"
#include <stdlib.h>
#include <math.h>

#define ID 1
#define PI 3.14159265358979323846

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

// threshold constants
const int IR_FRONT_REVERSE_THRESHOLD = 995;
const int IR_LEFT_REVERSE_THRESHOLD = 950;
const int IR_RIGHT_REVERSE_THRESHOLD = 950;
const int IR_FRONT_THRESHOLD = 950;
const int IR_STOP_TURNING_LEFTRIGHT_WHEN_FRONT_FREE_THRESHOLD = 920;
const int IR_FRONT_SIDE_THRESHOLD = 965;
const int IR_LEFT_THRESHOLD = 990;

// delay constants
const int LOOP_DELAY = 100;

const int MOVE_FORWARD_DELAY = 250;
const int FORWARD_AFTER_TURN_LEFT_DELAY = 250;
const int FORWARD_AFTER_TURN_RIGHT_DELAY = 250;
const int TURN_LEFT_DELAY = 100;
const int TURN_RIGHT_DELAY = 100;
const int REVERSE_DELAY = 400;

enum State
{
  turningLeft,
  turningRight,
  normal
};
// whether Mona robot is currently partway through turning or not
State state = normal;

// angle of 1 left/right rotation
const double ROTATION_UNIT = 4 * PI / 49;

// length of 1 forward
const double FORWARD_UNIT = 2.0 / 17.0;

double bearing = 0;

double x = 0;
double y = 0;

// using pin 7 for the CE pin, and pin 8 for the CSN pin
RF24 radio(7, 8);
uint8_t address[6] = "51423";
struct
{
  int id;
  int irLeft;
  int irLeftFront;
  int irFront;
  int irRightFront;
  int irRight;
  double x;
  double y;
  double bearing;
} payload;

void irRead()
{
  // IR enable
  digitalWrite(IR_ENABLE, HIGH);
  irRight = 0;
  irRightFront = 0;
  irFront = 0;
  irLeftFront = 0;
  irLeft = 0;

  // take NUM_OF_READINGS IR readings over a period of time and then average them
  for (int i = 0; i < NUM_OF_READINGS; i++)
  {
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

void forward()
{
  digitalWrite(LEFT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
  x += sin(bearing) * FORWARD_UNIT;
  y += cos(bearing) * FORWARD_UNIT;
}

void backward()
{
  digitalWrite(LEFT_MOTOR_DIRECTION, FORWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, FORWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
  x -= sin(bearing) * FORWARD_UNIT;
  y -= cos(bearing) * FORWARD_UNIT;
}

void left()
{
  digitalWrite(LEFT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, FORWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
  bearing -= ROTATION_UNIT;
  if (bearing < 0)
  {
    bearing += 2 * PI;
  }
}

void right()
{
  digitalWrite(LEFT_MOTOR_DIRECTION, FORWARD);
  analogWrite(LEFT_MOTOR_SPEED, LEFT_SPEED);
  digitalWrite(RIGHT_MOTOR_DIRECTION, BACKWARD);
  analogWrite(RIGHT_MOTOR_SPEED, RIGHT_SPEED);
  bearing += ROTATION_UNIT;
  if (bearing > 2 * PI)
  {
    bearing -= 2 * PI;
  }
}

void stop()
{
  // need to set direction to forwards for some reason
  digitalWrite(LEFT_MOTOR_DIRECTION, FORWARD);
  analogWrite(LEFT_MOTOR_SPEED, 0);
  digitalWrite(RIGHT_MOTOR_DIRECTION, FORWARD);
  analogWrite(RIGHT_MOTOR_SPEED, 0);
}

void move()
{
  // if the robot is already moving left, then finish moving left
  if (state == turningLeft)
  {
    left();
    delay(TURN_LEFT_DELAY);
    forward();
    x += sin(bearing) * FORWARD_UNIT * ((FORWARD_AFTER_TURN_LEFT_DELAY - LOOP_DELAY) / 100.0);
    y += cos(bearing) * FORWARD_UNIT * ((FORWARD_AFTER_TURN_LEFT_DELAY - LOOP_DELAY) / 100.0);
    delay(FORWARD_AFTER_TURN_LEFT_DELAY);
    if (irFront > IR_STOP_TURNING_LEFTRIGHT_WHEN_FRONT_FREE_THRESHOLD)
    {
      state = normal;
    }
  }
  else if (state == turningRight)
  {
    right();
    delay(TURN_RIGHT_DELAY);
    forward();
    x += sin(bearing) * FORWARD_UNIT * ((FORWARD_AFTER_TURN_RIGHT_DELAY - LOOP_DELAY) / 100.0);
    y += cos(bearing) * FORWARD_UNIT * ((FORWARD_AFTER_TURN_RIGHT_DELAY - LOOP_DELAY) / 100.0);
    delay(FORWARD_AFTER_TURN_RIGHT_DELAY);
    if (irFront > IR_STOP_TURNING_LEFTRIGHT_WHEN_FRONT_FREE_THRESHOLD)
    {
      state = normal;
    }
  }
  else
  {
    if ((irFront < IR_FRONT_REVERSE_THRESHOLD) && (irLeft < IR_LEFT_REVERSE_THRESHOLD) && (irRight < IR_RIGHT_REVERSE_THRESHOLD))
    {
      if (rand() < 0.8)
      {
        left();
        delay(REVERSE_DELAY);
        state = turningLeft;
      }
      else
      {
        forward();
        delay(MOVE_FORWARD_DELAY);
      }
    }

    else if (irLeftFront < IR_FRONT_SIDE_THRESHOLD)
    {
      right();
    }
    else if (irRightFront < IR_FRONT_SIDE_THRESHOLD)
    {
      left();
    }

    else if (irLeft > IR_LEFT_THRESHOLD)
    {
      forward();
      x += sin(bearing) * FORWARD_UNIT * ((MOVE_FORWARD_DELAY - LOOP_DELAY) / 100.0);
      y += cos(bearing) * FORWARD_UNIT * ((MOVE_FORWARD_DELAY - LOOP_DELAY) / 100.0);
      delay(MOVE_FORWARD_DELAY);
      left();
      state = turningLeft;
    }
    else if (irFront < IR_FRONT_THRESHOLD)
    {
      right();
      state = turningRight;
    }

    else
    {
      forward();
    }
  }
}

void sendInfo()
{
  payload.id = ID;
  payload.irLeft = irLeft;
  payload.irLeftFront = irLeftFront;
  payload.irFront = irFront;
  payload.irRightFront = irRightFront;
  payload.irRight = irRight;
  payload.x = x;
  payload.y = y;
  payload.bearing = bearing;

  unsigned long start_timer = micros();
  bool report = radio.write(&payload, sizeof(payload));
  unsigned long end_timer = micros();

  if (report)
  {
    Serial.print(F("Transmission successful! "));
    Serial.print(F("Time to transmit = "));
    Serial.print(end_timer - start_timer);
    Serial.println(F("Sent: "));
    Serial.print("irLeft: ");
    Serial.println(payload.irLeft);
    Serial.print("irLeftFront: ");
    Serial.println(payload.irLeftFront);
    Serial.print("irFront: ");
    Serial.println(payload.irFront);
    Serial.print("irRightFront: ");
    Serial.println(payload.irRightFront);
    Serial.print("irRight: ");
    Serial.println(payload.irRight);
    Serial.print("x: ");
    Serial.println(payload.x);
    Serial.print("y: ");
    Serial.println(payload.y);
    Serial.print("Bearing: ");
    Serial.println(payload.bearing);
  }
  else
  {
    Serial.println(F("Transmission failed or timed out"));
  }

  delay(1000);
}

// for debugging purposes
void printInfo()
{
  // Serial.println("----------------------");
  // Serial.print("ID: ");
  // Serial.println(ID);
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
  // Serial.print("State: ");
  // Serial.println(state);
  // Serial.print("x: ");
  // Serial.println(x);
  // Serial.print("y: ");
  // Serial.println(y);
  // Serial.print("Bearing: ");
  // Serial.println(bearing);
  // Serial.println("----------------------");
}

void setup()
{
  Serial.begin(9600);

  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  pinMode(IR_ENABLE, OUTPUT);
  pinMode(TOP_LED, OUTPUT);

  if (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!"));
    while (1)
    {
    }
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(payload));
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop()
{
  digitalWrite(TOP_LED, HIGH);
  irRead();
  move();
  // sendInfo();
  printInfo();
  digitalWrite(TOP_LED, LOW);
  delay(LOOP_DELAY);
}
