#include <SPI.h>
//#include "ps3_usb.h"
#include "WiiRemote.h"

int LATCH_PIN = 2;
int CLOCK_PIN = 3;
int DATA_PIN = 4;

volatile int index;
boolean buttons[16];

//PS3_USB PS3Game;
WiiRemote wiiremote;

void setup() {  
  attachInterrupt(0, resetButtons, RISING);
  attachInterrupt(1, snesKeyDown, RISING);
  
  pinMode(CLOCK_PIN, INPUT);
  pinMode(LATCH_PIN, INPUT);
  pinMode(DATA_PIN, OUTPUT);
  
  digitalWrite(CLOCK_PIN, HIGH);
  
  SPI.begin();
  
  //initPS3Controller();
  initWiiController();
}

//void initPS3Controller() {
//  Serial.begin(9600);
//  Serial.println("PS3 controller about to be initialized");
//  PS3Game.init();
//  Serial.println("PS3 controller initialized");
//}

void initWiiController() {
  wiiremote.init();
  
  unsigned char wiiremote_bdaddr[6] = {0x00, 0x1b, 0x7a, 0x00, 0x6c, 0xc5};
  wiiremote.setBDAddress(wiiremote_bdaddr, 6);
  wiiremote.setBDAddressMode(BD_ADDR_FIXED);
}

void loop() {
  // readControllerState();
  
  wiiremote.task(&readButtons);
}

void readButtons(void){
  buttons[0] = wiiremote.buttonPressed(WIIREMOTE_TWO);
  buttons[1] = wiiremote.buttonPressed(WIIREMOTE_ONE);
  buttons[2] = wiiremote.buttonPressed(WIIREMOTE_MINUS);
  buttons[3] = wiiremote.buttonPressed(WIIREMOTE_PLUS);
  buttons[4] = wiiremote.buttonPressed(WIIREMOTE_RIGHT);
  buttons[5] = wiiremote.buttonPressed(WIIREMOTE_LEFT);
  buttons[6] = wiiremote.buttonPressed(WIIREMOTE_UP);
  buttons[7] = wiiremote.buttonPressed(WIIREMOTE_DOWN);
  buttons[8] = wiiremote.buttonPressed(WIIREMOTE_A);
}

//void readControllerState() {
//  PS3Game.task();
//  if ((PS3Game.statConnected()) && (PS3Game.statReportReceived())){ // report received ?
//    if (PS3Game.buttonChanged()){
//      buttons[0] = PS3Game.buttonPressed(buCross);
//      buttons[1] = PS3Game.buttonPressed(buSquare);
//      buttons[2] = PS3Game.buttonPressed(buSelect);
//      buttons[3] = PS3Game.buttonPressed(buStart);
//      buttons[4] = PS3Game.buttonPressed(buUp);
//      buttons[5] = PS3Game.buttonPressed(buDown);
//      buttons[6] = PS3Game.buttonPressed(buLeft);
//      buttons[7] = PS3Game.buttonPressed(buRight);
//      buttons[8] = PS3Game.buttonPressed(buCircle);
//      buttons[9] = PS3Game.buttonPressed(buTriangle);
//      buttons[10] = PS3Game.buttonPressed(buL1);
//      buttons[11] = PS3Game.buttonPressed(buR1);
//      buttons[12] = false;
//      buttons[13] = false;
//      buttons[14] = false;
//      buttons[15] = false;
//    }
//  }
//}
//
//void readControllerStateNES(){
//  PS3Game.task();
//  if ((PS3Game.statConnected()) && (PS3Game.statReportReceived())){ // report received ?
//    if (PS3Game.buttonChanged()){
//      buttons[0] = PS3Game.buttonPressed(buCross);
//      buttons[1] = PS3Game.buttonPressed(buSquare);
//      buttons[2] = PS3Game.buttonPressed(buSelect);
//      buttons[3] = PS3Game.buttonPressed(buStart);
//      buttons[4] = PS3Game.buttonPressed(buUp);
//      buttons[5] = PS3Game.buttonPressed(buDown);
//      buttons[6] = PS3Game.buttonPressed(buLeft);
//      buttons[7] = PS3Game.buttonPressed(buRight);
//      buttons[8] = PS3Game.buttonPressed(buCircle);
//    }
//  }
//}

void snesKeyDown(){
  if (buttons[index] == 0) {
    PORTD |= B00010000; //turns signal to high
  } else {
    PORTD &= B11101111; //turns signal to low
  }
  index++;
}

void resetButtons(){
  index = 0;
  snesKeyDown();
}
