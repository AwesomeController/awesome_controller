#include <SPI.h>
#include "ps3_usb.h"

int clock = 2;
int latch = 3;
int data = 4;

boolean buttons[16];

PS3_USB PS3Game;

void setup() {
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(data, INPUT);
  
  digitalWrite(clock, HIGH);
  
  SPI.begin();
  
  initController();
}

void initController() {
  Serial.begin(9600);
  Serial.println("PS3 controller about to be initialized");
  PS3Game.init();
  Serial.println("PS3 controller initialized");
}

void loop() {
  readControllerState();
  
  digitalWrite(latch, HIGH);
  delayMicroseconds(12);
  digitalWrite(latch, LOW);
  delayMicroseconds(6);
  
  for (int i=0; i < 16; i++){
    digitalWrite(clock, LOW);
    buttons[i] = digitalRead(data);
    delayMicroseconds(6);
    
    digitalWrite(clock, HIGH);
    delayMicroseconds(6);
  }
  
  delay(17);
}

void readControllerState() {
  PS3Game.task();
  if ((PS3Game.statConnected()) && (PS3Game.statReportReceived())){ // report received ?
    if (PS3Game.buttonChanged()){
      buttons[0] = PS3Game.buttonPressed(buCross);
      buttons[1] = PS3Game.buttonPressed(buSquare);
      buttons[2] = PS3Game.buttonPressed(buSelect);
      buttons[3] = PS3Game.buttonPressed(buStart);
      buttons[4] = PS3Game.buttonPressed(buUp);
      buttons[5] = PS3Game.buttonPressed(buDown);
      buttons[6] = PS3Game.buttonPressed(buLeft);
      buttons[7] = PS3Game.buttonPressed(buRight);
      buttons[8] = PS3Game.buttonPressed(buCircle);
      buttons[9] = PS3Game.buttonPressed(buTriangle);
      buttons[10] = PS3Game.buttonPressed(buL1);
      buttons[11] = PS3Game.buttonPressed(buR1);
      buttons[12] = false;
      buttons[13] = false;
      buttons[14] = false;
      buttons[15] = false;
      
      int len = sizeof(buttons)/sizeof(boolean);

      for (int i = 0; i < len; i++){
        Serial.print("Button #");
        Serial.print(i);
        Serial.print(": ");
        Serial.println((int)buttons[i]);
      }
    }
  }
}
