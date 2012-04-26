#include <Arduino.h>
#include "WiiController.h"

WiiController::WiiController(void) {
    Serial.println("in WiiController constuctor");
    for (int i = 0; i < 16; i++) {
        buttons[i] = 0;
    }
    Serial.println("done in WiiController constuctor");
}

WiiController::~WiiController(void) {
}

void WiiController::printButtonStates(void) {
    Serial.print("Button states: ");
    for (int i = 0; i < 16; i++) {
        Serial.print(buttons[i]);
    }
    Serial.println("");
}