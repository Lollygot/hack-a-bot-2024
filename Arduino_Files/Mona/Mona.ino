#include <MsTimer2.h>

// Variables for 5 IR proximity sensors
int irRight, irRightFront, irFront, irLeftFront, irLeft;

// number of times to poll IR sensors over period of time
const int NUM_OF_READINGS = 5;

// pin mappings
const int IR_ENABLE = 4;
const int TOP_LED = 13;

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

void setup() {
  Serial.begin(9600);
}

void loop() {
  digitalWrite(TOP_LED, HIGH);
  IR_proximity_read();
  digitalWrite(TOP_LED, LOW);
  delay(100);
}
