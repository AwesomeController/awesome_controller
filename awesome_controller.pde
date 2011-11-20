#include <SPI.h>
#include "ps3_usb.h"

int LATCH_PIN = 2;
int CLOCK_PIN = 3;
int DATA_PIN = 4;

volatile int index;
volatile int counter;
boolean buttons[16];
boolean buttons_feedback[16];

PS3_USB PS3Game;

void setup() {
  attachInterrupt(0, resetButtons, RISING);
  attachInterrupt(1, snesKeyDown, RISING);
  
  pinMode(CLOCK_PIN, INPUT);
  pinMode(LATCH_PIN, INPUT);
  pinMode(DATA_PIN, OUTPUT);
  
  digitalWrite(CLOCK_PIN, HIGH);
  
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
  readControllerStateNES();
//  if (counter > 64) {
//    counter = 0;
//    for (int i = 0; i < 16; i++) {
//    Serial.print(i);
//    Serial.print(": ");
//    Serial.println((int)buttons_feedback[i]);
//    }
//    Serial.println("latch");
//  }
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
    }
  }
}

void readControllerStateNES(){
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
    }
  }
}

void snesKeyDown(){
  if (buttons[index] == 0) {
    PORTD |= B00010000; //turns signal to high
  } else {
    PORTD &= B11101111; //turns signal to low
  }
  buttons_feedback[index] = buttons[index];
  index++;
}

void resetButtons(){
  index = 0;
  snesKeyDown();
  counter++;
}
