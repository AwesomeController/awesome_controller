#include <SPI.h>
#include "ps3_usb.h"
#include "WiiController.h"
#include "BluetoothUsbHostHandler.h"
#include "n64.h"
#include "nes.h"

#define CONSOLE_NES  1
#define CONSOLE_SNES 2
#define CONSOLE_N64  3
#define CONSOLE_CHOICE CONSOLE_NES

#define NOOP_FOR_2_US asm volatile("nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n")

int buttonStatePrintCounter = 0;

PS3_USB PS3Game;
BluetoothUsbHostHandler bluetoothUsbHostHandler;
WiiController wiiController;
N64 n64system;
NES nesSystem;

void setup()
{
    Serial.begin(9600);

    if (CONSOLE_CHOICE == CONSOLE_NES) {
        nesSystem.init();
    } else if (CONSOLE_CHOICE == CONSOLE_SNES) {
        // create SNES system
    } else if (CONSOLE_CHOICE == CONSOLE_N64) {
        n64system.init();
    } else {
        Serial.println("Console choice not recognized.");
        exit(-1);
    }

    SPI.begin();
    PS3Game.init();
    bluetoothUsbHostHandler.init();
    bluetoothUsbHostHandler.setBDAddressMode(BD_ADDR_INQUIRY);
}

void loop() {
    if (CONSOLE_CHOICE == CONSOLE_NES) {
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
    } else if (CONSOLE_CHOICE == CONSOLE_SNES) {
        // handle SNES console
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
