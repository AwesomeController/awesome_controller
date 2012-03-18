#include <SPI.h>
#include "ps3_usb.h"
#include "WiiController.h"
#include "BluetoothUsbHostHandler.h"
#include "n64.h"

#define SYSTEM_NONE 0
#define SYSTEM_NES  1
#define SYSTEM_SNES 2
#define SYSTEM_N64  3

int LATCH_PIN = 2;
int CLOCK_PIN = 3;
int DATA_PIN = 4;
int currentlyConnectedSystem = SYSTEM_NONE;

volatile int buttonCyclesSinceLatch;
int buttonStatePrintCounter = 0;

PS3_USB PS3Game;
BluetoothUsbHostHandler bluetoothUsbHostHandler;
WiiController wiiController;
N64 n64system;

void setup()
{
    Serial.begin(9600);
}

void initPS3Controller() {
  PS3Game.init();
}

void initBluetoothUsbHostHandler()
{
  bluetoothUsbHostHandler.init(wiiController);
  bluetoothUsbHostHandler.setBDAddressMode(BD_ADDR_INQUIRY);
}


int pollConnectedSystem()
{
    // TODO: this is what Kyle will figure out based on the analog
    // value (resistance)
    return SYSTEM_N64;
}

// Given a system, perform any teardown of pins, etc. when we
// disconnect its connector.
void systemDown(int system)
{
    if (system == SYSTEM_NONE) {
        return;
    }

    if (system == SYSTEM_NES || system == SYSTEM_SNES) {
    }

    if (system == SYSTEM_N64) {
    }
}

// Given a system, perform any setup of pins, etc. when we
// connect its connector.
void systemUp(int system)
{
    if (system == SYSTEM_NONE) {
        return;
    }

    if (system == SYSTEM_NES || system == SYSTEM_SNES) {
        attachInterrupt(0, resetButtons, RISING);
        attachInterrupt(1, snesKeyDown, RISING);

        // Setup clock, latch, and data pins for SNES/NES
        pinMode(CLOCK_PIN, INPUT);
        pinMode(LATCH_PIN, INPUT);
        pinMode(DATA_PIN, OUTPUT);

        // Initialize clock pin to 5 volts
        digitalWrite(CLOCK_PIN, HIGH);

        SPI.begin();
        initPS3Controller();
        initBluetoothUsbHostHandler();
    }

    if (currentlyConnectedSystem == SYSTEM_N64) {
        SPI.begin();
        initPS3Controller();
        initBluetoothUsbHostHandler();

        n64system.init();
    }
}

void seeIfSystemChanged()
{
    int polledSystemConnection = pollConnectedSystem();

    if (currentlyConnectedSystem != polledSystemConnection) {
        // if in here, we changed systems

        // do the down action for the previous system
        systemDown(currentlyConnectedSystem);

        currentlyConnectedSystem = polledSystemConnection;

        // do the up action for the new system
        systemUp(currentlyConnectedSystem);
    }
}

void loop()
{
    seeIfSystemChanged();
    handleButtons();
}

void handleButtons()
{
    if (currentlyConnectedSystem == SYSTEM_NES || currentlyConnectedSystem == SYSTEM_SNES) {
        // eventually: for each controller, read their state and store.
        // right now only works for the one controller that is plugged in
        readControllerState();

        // eventually: for each wiimote, read their state and store.
        // right now only works for the one controller that is plugged in
        bluetoothUsbHostHandler.task(&readButtons);

        buttonStatePrintCounter++;
    } else if (currentlyConnectedSystem == SYSTEM_N64) {
        // eventually: for each controller, read their state and store.
        // right now only works for the one controller that is plugged in
        //readControllerState();

        // eventually: for each wiimote, read their state and store.
        // right now only works for the one controller that is plugged in
        bluetoothUsbHostHandler.task(&readButtons);

        buttonStatePrintCounter++;
    }

    if (buttonStatePrintCounter > 250) {
        //wiiController.printButtonStates();
        buttonStatePrintCounter = 0;
    }
}

void readButtons(void)
{
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

void readControllerState()
{
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

void snesKeyDown()
{
    // debugging code
    //if (wiiController.buttons[buttonCyclesSinceLatch] == 0) {
    //    RED_LED_ON;
    //} else {
    //    RED_LED_OFF;
    //}
    buttonCyclesSinceLatch++;
}

void resetButtons()
{
    buttonCyclesSinceLatch = 0;
    snesKeyDown();
}
