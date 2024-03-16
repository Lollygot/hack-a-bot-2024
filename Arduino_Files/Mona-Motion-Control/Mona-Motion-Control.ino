//Motion Control for Open-loop and Close-Lopp
#include <MsTimer2.h>
#include "RF24.h"
// pin config for basic platform test
// Motors
int Motor_right_PWM = 10;  //   0 (min speed) - 255 (max speed)
int Motor_right_direction = 5;  //   0 Forward - 1 Reverse
int Motor_left_PWM = 9;    //   0 (min speed) - 255 (max speed)
int Motor_left_direction = 6;   //   0 Forward - 1 Reverse
#define Forward 0
#define Reverse 1
// LED
int LED1 = 13;
int LED2 = 12;
int IR_enable=4;
int IR_threshold= 900; // 0 white close obstacle -- 1023 no obstacle

const byte Left_interruptPin = 2;
const byte Right_interruptPin = 3;

int Left_forward_speed=111;
int Right_forward_speed=111;
float slope = 0.1;

// instantiate an object for the nRF24L01 transceiver
RF24 radio(7, 8);  // using pin 7 for the CE pin, and pin 8 for the CSN pin

// Let these addresses be used for the pair
uint8_t address[][6] = { "SomeRandomAddress1", "SomeRandomAddress2" };
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit
bool radioNumber = 1;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit

// Used to control whether this node is sending or receiving
bool role = true;  // true = TX role, false = RX role

// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
float payload = 0.0;


void forward(){
  analogWrite(Motor_right_PWM,Right_forward_speed); // right motor
  digitalWrite(Motor_right_direction,Forward); //right
  analogWrite(Motor_left_PWM,Left_forward_speed); // left
  digitalWrite(Motor_left_direction,Forward); //left
}

void Stop(){ // set speeds to 0
  analogWrite(Motor_right_PWM,0); // right motor
  analogWrite(Motor_left_PWM, 0); // left
  MsTimer2::start();
}

// Variables for 5 IR proximity sensors
int IR_right,IR_right_front,IR_front,IR_left_front,IR_left;

void IR_proximity_read(){    // read only front IR sensor
  int n=5;  // average parameter
  digitalWrite(IR_enable, HIGH);  //IR Enable
  IR_right=0;
  IR_right_front=0;
  IR_front=0;
  IR_left_front=0;
  IR_left=0;
  for (int i=0;i<n;i++){  //read from front proximity sensor
    IR_right+=analogRead(A3);
    IR_right_front+=analogRead(A2);
    IR_front+=analogRead(A1);
    IR_left_front+=analogRead(A0);
    IR_left+=analogRead(A7);
    delay(20);
  }
  IR_right/=n;
  IR_right_front/=n;
  IR_front/=n;
  IR_left_front/=n;
  IR_left/=n;
}

void Obstacle_avoidance(){
  if (IR_front<IR_threshold || IR_right<IR_threshold || IR_right_front<IR_threshold || IR_left<IR_threshold || IR_left_front<IR_threshold){
      digitalWrite(LED1,HIGH);
      Stop();
  }
}

long int Left_counter, Right_counter;

void Left_int_counter(){
  Left_counter++;
}

void Right_int_counter(){
  Right_counter++;
}

float Left_compensatory=1,Right_compensatory=1;
int Desired = 910;   // number of pulses which is desired
int run_exp=0;

void Timer_overflow() {   // every 400 ms is called
  run_exp++;
  Right_compensatory =  Desired - Right_counter;
  Left_compensatory  =  Desired - Left_counter;

  Serial.print("Left = ");
  Serial.print(Left_counter);
  Serial.print(" , ");
  Serial.print("  Right = ");
  Serial.println(Right_counter);

  // a basic proportional control (disable these two lines for Open-loop control)
  Right_forward_speed +=  slope * Right_compensatory;
  Left_forward_speed  +=  slope *  Left_compensatory;

  forward();  // update PWM set-point

  //reset counters
  Left_counter=0;
  Right_counter=0;
}

void setup() {
 Serial.begin(9600);
  pinMode(Motor_left_PWM, OUTPUT);
  pinMode(Motor_right_PWM, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(IR_enable, OUTPUT);
  // set encoder counter to 0
  Left_counter=0;
  Right_counter=0;

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  radioNumber = false;

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(payload));  // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1

  // additional setup specific to the node's role
  radio.stopListening();  // put radio in TX mode

  // init INT0 and INT1 for left and right motors encoders
  pinMode(Left_interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Left_interruptPin), Left_int_counter, CHANGE);
  pinMode(Right_interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Right_interruptPin), Right_int_counter, CHANGE);
  // setup Timer2 for motor control
  MsTimer2::set(400, Timer_overflow); // 400ms period
  MsTimer2::start();
  //set PWM set-point
  forward();
}

//------------------------------------------------- Main loop
void loop() {
 digitalWrite(LED1,HIGH); //Top LED
 IR_proximity_read();
 Obstacle_avoidance();
 digitalWrite(LED1,LOW); //Top LED

  unsigned long start_timer = micros();                // start the timer
  bool report = radio.write(&payload, sizeof(float));  // transmit & save the report
  unsigned long end_timer = micros();                  // end the timer

  if (report) {
    Serial.print(F("Transmission successful! "));  // payload was delivered
    Serial.print(F("Time to transmit = "));
    Serial.print(end_timer - start_timer);  // print the timer result
    Serial.print(F(" us. Sent: "));
    Serial.println(payload);  // print payload sent
    payload += 0.01;          // increment float payload
  } else {
    Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
  }

  // to make this example readable in the serial monitor
  delay(1000);  // slow transmissions down by 1 second

//  delay(100);
}
