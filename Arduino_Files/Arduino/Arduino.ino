#include <SPI.h>
#include "RF24.h"

// using pin 7 for the CE pin, and pin 8 for the CSN pin
RF24 radio(7, 8);
uint8_t address[6] = "51423";
struct {
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

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(payload));
  radio.openReadingPipe(1, address);
  radio.startListening();

}

void loop() {
  if (radio.available()) {
    uint8_t bytes = radio.getPayloadSize();
    radio.read(&payload, bytes);
    // avoid blocking calls
    if (Serial.availableForWrite() >= sizeof(payload)) {
      Serial.write(reinterpret_cast<char*>(&payload), sizeof(payload));

      // for debugging bytes sent over
      // char* temp = static_cast<char*>(static_cast<void*>(&payload));
      // for (int i = 0; i < sizeof(payload); i++) {
      //   for (int j = 8 * sizeof(char) - 1; j >= 0; j--) {
      //     Serial.print((temp[i] >> j) & 1);
      //   }
      //   Serial.print(" ");
      // }
      // Serial.println();
    }
    // for debugging values received
    // Serial.print(F("Received "));
    // Serial.print(bytes);
    // Serial.println(F(" bytes:"));
    // Serial.print("irLeft: ");
    // Serial.println(payload.irLeft);
    // Serial.print("irLeftFront: ");
    // Serial.println(payload.irLeftFront);
    // Serial.print("irFront: ");
    // Serial.println(payload.irFront);
    // Serial.print("irRightFront: ");
    // Serial.println(payload.irRightFront);
    // Serial.print("irRight: ");
    // Serial.println(payload.irRight);
    // Serial.print("x: ");
    // Serial.println(payload.x);
    // Serial.print("y: ");
    // Serial.println(payload.y);
    // Serial.print("Bearing: ");
    // Serial.println(payload.bearing);
  }
}
