#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#define LED_PIN 13
#define SIGNAL_PIN 8
#define ADC_PIN A0
#define MAX_SAMPLES 1024

char messageFlag = 'O';
int messageCounter = 0;
char cmd;
char codedSignal[2];
char codedTime[5];
int signalPeriod;
int signalCount = 0;
int samplesCount = 0;
char tempCodedSample[2];
unsigned char injectedSignal[MAX_SAMPLES/8];
unsigned char collectedSamples[MAX_SAMPLES];



void setup() {
  Serial.begin(9600);
  pinMode(SIGNAL_PIN, OUTPUT);
  digitalWrite(SIGNAL_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}



void setFlag(char rawCmd) {
  if (rawCmd == 'T') {
    messageFlag = 'T';
    messageCounter = 3;
  }
  else if (rawCmd == 'S') {
    messageFlag = 'S';
    messageCounter = 1;
    signalCount = 0;
    samplesCount = 0;
  }
  else if (rawCmd == 'X') {
    messageFlag = 'X';
    messageCounter = 0;
    samplesCount = signalCount * 8;
  }
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
  injectedSignal[signalCount] = decodeHexedSignal(codedSignal);
  signalCount++;
  messageCounter = 1;
}



void decodeTime() {
  codedTime[4] = '\0';
  signalPeriod = atoi(codedTime);
}



void collectSamples() {
  digitalWrite(LED_PIN, HIGH);
  for( int i = 0; i < samplesCount; i++ ) {
    collectedSamples[i] = analogRead(ADC_PIN);
    if( injectedSignal[ i >> 3 ] & (0x80 >> (i & 7)) ) {
      digitalWrite(SIGNAL_PIN, HIGH);
    }
    else {
      digitalWrite(SIGNAL_PIN, LOW);
    }
    delay(signalPeriod);
  }
  digitalWrite(LED_PIN, LOW);
}


void sendSampledData() {
  Serial.print("T");
  for (int i = 0; i < samplesCount; i++) {
    sprintf(tempCodedSample, "%02x", collectedSamples[i]);
    Serial.print(tempCodedSample);
  }
  Serial.print("X");
  messageFlag = 'O';
}


void loop() {
  while(!Serial.available());
  cmd = Serial.read();
  setFlag(cmd);
  if ((messageFlag == 'T') && (cmd >= '0' && cmd <= '9') && (messageCounter >= 0)) {
    codedTime[3-messageCounter] = cmd;
    messageCounter--;
  }
  if ((messageFlag == 'T') && (messageCounter == -1)) {
    decodeTime();
  }
  if ((messageFlag == 'S') && ((cmd >= '0' && cmd <= '9') || (cmd >= 'a' && cmd <= 'f')) && (messageCounter >= 0)) {
    codedSignal[1-messageCounter] = cmd;
    messageCounter--;
  }
  if ((messageFlag == 'S') && (messageCounter == -1)) {
    decodeSignal();
  }
  if (messageFlag == 'X') {
    Serial.flush();
    delay(0.1);
    collectSamples();
    delay(0.1);
    sendSampledData();
  }
}
