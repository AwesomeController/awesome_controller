#include "nes.h"
#include "WiiController.h"

extern WiiController wiiController;

void caughtClock1() { /* noop */ }

// Handle full cycle from one latch until all 8 buttons have been sent
// or we have gone more than 12 nanoseconds without seeing another
// clock or latch.
void handleLatchCycle() {
    int loopsSinceClock = 0;
    int clock1ButtonsSinceLatch = 1;
    int clock2ButtonsSinceLatch = 1;

    // Immediately send the first button on a clock signal.
    if (wiiController.buttons[0]) {
        PORTD &= B11101111; // turns signal to low
        //PORTD &= B10111111; // turns signal to low
    } else {
        PORTD |= B00010000; // turns signal to high
        //PORTD |= B01000000; // turns signal to high
    }

    // We want to see 8 clock cycles total, and we have already sent
    // our first button. So we poll until we see the interrupt register bit set
    // and this indicates that a clock cycle has occurred (we should send the
    // next button in anticipation.)
    while (clock1ButtonsSinceLatch < 9 && clock2ButtonsSinceLatch < 9) {
        // clock 1
        if (clock1ButtonsSinceLatch < 9) {
            if (EIFR & 0x01) { // interrupt is high
                // Toggle interrupt handler to clear additional interrupts
                // that occurred during this ISR.
                EIFR |= (1 << INTF0);

                if (clock1ButtonsSinceLatch == 8) {
                    // On our last cycle, we have already sent 8 buttons, so we
                    // should reset state and prepare to leave the ISR.
                    PORTD &= B11101111; // turns signal to low
                } else if (wiiController.buttons[clock1ButtonsSinceLatch]) {
                    PORTD &= B11101111; // turns signal to low
                } else {
                    PORTD |= B00010000; // turns signal to high
                }

                clock1ButtonsSinceLatch++;
                loopsSinceClock = 0;
            }
        }

        // clock 2
        if (clock2ButtonsSinceLatch < 9) {
            if (EIFR & 0x01) { // "interrupt" is high
                // Toggle interrupt handler to clear additional interrupts
                // that occurred during this ISR.
                //EIFR |= (1 << INTF1);

                if (clock2ButtonsSinceLatch == 8) {
                    // On our last cycle, we have already sent 8 buttons, so we
                    // should reset state and prepare to leave the ISR.
                    //PORTD &= B10111111; // turns signal to low
                } else if (wiiController.buttons[clock2ButtonsSinceLatch]) {
                    //PORTD &= B10111111; // turns signal to low
                } else {
                    //PORTD |= B01000000; // turns signal to high
                }

                clock2ButtonsSinceLatch++;
                loopsSinceClock = 0;
            }
        }

        loopsSinceClock++;

        if (loopsSinceClock > 60) {
            // We timed out because there were no clock cycles recently,
            // so we should reset state and prepare to leave the ISR.
            if (clock1ButtonsSinceLatch == 8) {
                PORTD |= B01000000; // blue/white led to high
            }
            PORTD |= B00100000; // red led on
            break;
        }
    }

    PORTD &= B11101111; // turns signal low
    //PORTD &= B10111111; // turns led low

    // Toggle interrupt handler to clear additional interrupts
    // that occurred during this ISR.
    EIFR |= (1 << INTF1);
    EIFR |= (1 << INTF0);
}

void NES::init() {
    CLATCH_PIN = 2;
    CLOCK_PIN = 3;
    DATA_PIN = 4;

    attachInterrupt(0, handleLatchCycle, FALLING);
    attachInterrupt(1, caughtClock1, FALLING);

    // Setup clock latch and data pins for SNES/NES
    pinMode(CLOCK_PIN, INPUT);
    pinMode(CLATCH_PIN, INPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);

    //Initialize clock pin to 5 volts
    digitalWrite(DATA_PIN, LOW);
}

NES::NES() {
}

NES::~NES() {
}
