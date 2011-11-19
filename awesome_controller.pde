#include <SPI.h>
#include "ps3_usb.h"

int clock = 2;
int latch = 3;
int data = 4;

int buttons[16];

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
  Serial.println("Reading controller state");
  PS3Game.task();
  if ((PS3Game.statConnected()) && (PS3Game.statReportReceived())){ // report received ?
    if(PS3Game.buttonChanged()){
      if(PS3Game.buttonPressed(buLeft)) {
        Serial.println("Pressed left!");
      }
    }
  }
}
