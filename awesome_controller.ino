#include <SPI.h>
#include "ps3_usb.h"
#include "BluetoothUsbHostHandler.h"

int LATCH_PIN = 2;
int CLOCK_PIN = 3;
int DATA_PIN = 4;

volatile int index;
boolean buttons[16];

PS3_USB PS3Game;
BluetoothUsbHostHandler bluetoothUsbHostHandler;

void setup() {
  attachInterrupt(0, resetButtons, RISING);
  attachInterrupt(1, snesKeyDown, RISING);

  pinMode(CLOCK_PIN, INPUT);
  pinMode(LATCH_PIN, INPUT);
  pinMode(DATA_PIN, OUTPUT);

  digitalWrite(CLOCK_PIN, HIGH);

  SPI.begin();
  Serial.begin(9600);

  initPS3Controller();
  initBluetoothUsbHostHandler();
}

void initPS3Controller() {
  Serial.println("PS3 USB library about to be initialized");
  PS3Game.init();
  Serial.println("PS3 USB library initialized");
}

void initBluetoothUsbHostHandler() {
  Serial.println("Bluetooth USB Host Handler to be initialized");
  bluetoothUsbHostHandler.init();
  Serial.println("Bluetooth USB Host Handler initialized");
  bluetoothUsbHostHandler.setBDAddressMode(BD_ADDR_INQUIRY);
}

void loop() {
  // eventually: for each controller, read their state and store.
  // right now only works for the one controller that is plugged in
  readControllerState();

  // eventually: for each wiimote, read their state and store.
  // right now only works for the one controller that is plugged in
  bluetoothUsbHostHandler.task(&readButtons);
}

void readButtons(void){
  buttons[0] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_TWO);
  buttons[1] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_ONE);
  buttons[2] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_MINUS);
  buttons[3] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_PLUS);
  buttons[4] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_RIGHT);
  buttons[5] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_LEFT);
  buttons[6] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_UP);
  buttons[7] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_DOWN);
  buttons[8] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_A);
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
    }
  }
}

// not needed at this moment (since NES ~= SNES) but might be a good starting point
// for having different controller types, etc.
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

void snesKeyDown() {
  if (buttons[index] == 0) {
    PORTD |= B00010000; // turns signal to high
  } else {
    PORTD &= B11101111; // turns signal to low
  }
  index++;
}

void resetButtons() {
  index = 0;
  snesKeyDown();
}
