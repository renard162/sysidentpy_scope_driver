#include <Arduino.h>

int ByteReceived;
byte inputArray[10000];
byte outputArray[80000];

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    ByteReceived = Serial.read();
    Serial.print(ByteReceived);
    Serial.println();
  }
}
