#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
// #include <math.h>

char messageFlag = 'O';
int messageCounter = 0;
char cmd;
char codedSignal[2];
char codedTime[5];
int signalPeriod;
int signalCount;
unsigned char injectedSignal[32];
unsigned char collectedSamples[256];

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}


int decodeHexedSignal(char *hex) {
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


void decodeSignal() {
  injectedSignal[messageCounter] = decodeHexedSignal(codedSignal);
  Serial.print(injectedSignal[messageCounter]);
  Serial.println();
  messageCounter++;
  messageCounter = 1;
}


void setFlag(char rawCmd) {
  if (rawCmd == 'T') {
    messageFlag = 'T';
    messageCounter = 3;
    Serial.print(rawCmd);
    Serial.println();
  }
  else if (rawCmd == 'S') {
    messageFlag = 'S';
    messageCounter = 1;
    signalCount = 0;
    Serial.print(rawCmd);
    Serial.println();
  }
  else if (rawCmd == 'Z') {
    messageFlag = 'Z';
    messageCounter = 0;
    Serial.print(rawCmd);
    Serial.println();
  }
}


void decodeTime() {
  codedTime[4] = '\0';
  signalPeriod = atoi(codedTime);
  Serial.print(signalPeriod);
  Serial.println();
}


void executeExperiment() {

}


void loop() {
  while(!Serial.available());
  cmd = Serial.read();
  setFlag(cmd);
  if ((messageFlag == 'T') && (cmd >= '0' && cmd <= '9') && (messageCounter >= 0)) {
    codedTime[messageCounter] = cmd;
    messageCounter--;
  }
  if ((messageFlag == 'T') && (messageCounter == -1)) {
    decodeTime();
  }
  if ((messageFlag == 'S') && ((cmd >= '0' && cmd <= '9') || (cmd >= 'a' && cmd <= 'f')) && (messageCounter >= 0)) {
    codedSignal[messageCounter] = cmd;
    messageCounter--;
  }
  if ((messageFlag == 'S') && (messageCounter == -1)) {
    decodeSignal();
  }
  if (messageFlag == 'Z') {
    Serial.flush();
    executeExperiment()
  }
  // if (cmd == 'l') {
  //   digitalWrite(13, HIGH); 
  // }
  // else if (cmd == 'd') {
  //   digitalWrite(13, LOW);
  // }
}
