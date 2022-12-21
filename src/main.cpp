#include <Arduino.h>
// #include <stdio.h>
// #include <string.h>
// #include <math.h>

char cmd;
unsigned char message[2];
int messageCounter;
unsigned char signal[32];
unsigned char samples[256];

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

int b8_hex2int(unsigned char *hex){
   int dec = 0;
   int x, i;
   for (i = 1 ; i >= 0 ; --i) {
      if (hex[i] >= '0' && hex[i] <= '9') {
         x = hex[i] - '0';
      }
      else if (hex[i] >= 'a' && hex[i] <= 'f') {
         x = hex[i] - 'a' + 10;
      }
      dec = dec + x * pow(16 , 1-i);
   }
   return dec;
}

void loop() {
  messageCounter = 1;
  while(!Serial.available());
  while (messageCounter >= 0) {
    cmd = Serial.read();
    if ((cmd >= '0' && cmd <= '9') || (cmd >= 'a' && cmd <= 'f')) {
      message[messageCounter] = cmd;
      messageCounter--;
    }
  }
  messageCounter = 1;
  Serial.print(b8_hex2int(message));
  Serial.println();
  // if (cmd == 'l') {
  //   digitalWrite(13, HIGH); 
  // }
  // else if (cmd == 'd') {
  //   digitalWrite(13, LOW);
  // }
}
