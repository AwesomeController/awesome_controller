#include <SPI.h>
#include <wiring.h>
#include "ps3_usb.h"
#include "WiiController.h"
#include "BluetoothUsbHostHandler.h"
#include "n64.h"

#define CONSOLE_NES  1
#define CONSOLE_SNES 2
#define CONSOLE_N64  3
#define CONSOLE_CHOICE CONSOLE_N64

int LATCH_PIN = 2;
int CLOCK_PIN = 3;
int DATA_PIN = 4;

volatile int buttonCyclesSinceLatch;
int buttonStatePrintCounter = 0;

PS3_USB PS3Game;
BluetoothUsbHostHandler bluetoothUsbHostHandler;
WiiController wiiController;
N64 n64system;

void setup();
void loop();
void initPS3Controller();
void initBluetoothUsbHostHandler();
void readButtons(void);
void readControllerState();
void resetButtons();
void snesKeyDown();

void setup()
{
    Serial.begin(9600);
    if (CONSOLE_CHOICE == CONSOLE_NES || CONSOLE_CHOICE == CONSOLE_SNES) {
        attachInterrupt(0, resetButtons, RISING);
        attachInterrupt(1, snesKeyDown, RISING);

        // Setup clock latch and data pins for SNES/NES
        pinMode(CLOCK_PIN, INPUT);
        pinMode(LATCH_PIN, INPUT);
        pinMode(DATA_PIN, OUTPUT);

        //Initialize clock pin to 5 volts
        digitalWrite(CLOCK_PIN, HIGH);
        SPI.begin();

        initPS3Controller();
        initBluetoothUsbHostHandler();
    } else if (CONSOLE_CHOICE == CONSOLE_N64) {
        SPI.begin();
        initPS3Controller();
        initBluetoothUsbHostHandler();

        n64system.init();
    }
}

void initPS3Controller() {
  PS3Game.init();
}

void initBluetoothUsbHostHandler() {
  bluetoothUsbHostHandler.init();
  bluetoothUsbHostHandler.setBDAddressMode(BD_ADDR_INQUIRY);
}

void loop() {
    //Serial.println("at top of loop");
    if (CONSOLE_CHOICE == CONSOLE_NES || CONSOLE_CHOICE == CONSOLE_SNES) {
        // eventually: for each controller, read their state and store.
        // right now only works for the one controller that is plugged in
        readControllerState();

        // eventually: for each wiimote, read their state and store.
        // right now only works for the one controller that is plugged in
        bluetoothUsbHostHandler.task(&readButtons);

        buttonStatePrintCounter++;
        if (buttonStatePrintCounter > 250) {
            wiiController.printButtonStates();
            buttonStatePrintCounter = 0;
        }
    } else if (CONSOLE_CHOICE == CONSOLE_N64) {
        // eventually: for each controller, read their state and store.
        // right now only works for the one controller that is plugged in
        //readControllerState();

        // eventually: for each wiimote, read their state and store.
        // right now only works for the one controller that is plugged in
        bluetoothUsbHostHandler.task(&readButtons);

        buttonStatePrintCounter++;
        if (buttonStatePrintCounter > 250) {
            //wiiController.printButtonStates();
            buttonStatePrintCounter = 0;
        }
    }
}

void readButtons(void) {
    wiiController.buttons[0] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_TWO);
    wiiController.buttons[1] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_ONE);
    wiiController.buttons[2] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_B);
    wiiController.buttons[3] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_PLUS);
    wiiController.buttons[4] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_RIGHT);
    wiiController.buttons[5] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_LEFT);
    wiiController.buttons[6] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_UP);
    wiiController.buttons[7] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_DOWN);
    if (wiiController.buttons[0] ||
        wiiController.buttons[1] ||
        wiiController.buttons[2] ||
        wiiController.buttons[3] ||
        wiiController.buttons[4] ||
        wiiController.buttons[5] ||
        wiiController.buttons[6] ||
        wiiController.buttons[7]) {
        RED_LED_ON;
    } else {
        RED_LED_OFF;
    }
}

void readControllerState() {
    PS3Game.task();
    if ((PS3Game.statConnected()) && (PS3Game.statReportReceived())){ // report received ?
        if (PS3Game.buttonChanged()){
            wiiController.buttons[0] = PS3Game.buttonPressed(buCross);
            wiiController.buttons[1] = PS3Game.buttonPressed(buSquare);
            wiiController.buttons[2] = PS3Game.buttonPressed(buSelect);
            wiiController.buttons[3] = PS3Game.buttonPressed(buStart);
            wiiController.buttons[4] = PS3Game.buttonPressed(buUp);
            wiiController.buttons[5] = PS3Game.buttonPressed(buDown);
            wiiController.buttons[6] = PS3Game.buttonPressed(buLeft);
            wiiController.buttons[7] = PS3Game.buttonPressed(buRight);
            wiiController.buttons[8] = PS3Game.buttonPressed(buCircle);
            wiiController.buttons[9] = PS3Game.buttonPressed(buTriangle);
            wiiController.buttons[10] = PS3Game.buttonPressed(buL1);
            wiiController.buttons[11] = PS3Game.buttonPressed(buR1);
            wiiController.buttons[12] = false;
            wiiController.buttons[13] = false;
            wiiController.buttons[14] = false;
            wiiController.buttons[15] = false;
        }
    }
}

void snesKeyDown() {
    if (wiiController.buttons[buttonCyclesSinceLatch] == 0) {
        RED_LED_ON;
    } else {
        RED_LED_OFF;
    }
    buttonCyclesSinceLatch++;
}

void resetButtons() {
    buttonCyclesSinceLatch = 0;
    snesKeyDown();
}
