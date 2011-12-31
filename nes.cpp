#include "nes.h"
#include "WiiController.h"


#define NES_SET_DATA1_HIGH PORTD |= B00010000
#define NES_SET_DATA1_LOW  PORTD &= B11101111
#define NES_CLEAR_CLATCH_INTERRUPT EIFR |= (1 << INTF0);
#define NES_CLATCH_INTERRUPT_IS_HIGH EIFR & 0x01

#define NES_SET_DATA2_HIGH PORTD |= B00001000
#define NES_SET_DATA2_LOW  PORTD &= B11110111
#define NES_CLEAR_CLOCK2_INTERRUPT EIFR |= (1 << INTF1);
#define NES_CLOCK2_INTERRUPT_IS_HIGH EIFR & 0x02

#define NOOP_FOR_2_US asm volatile("nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n" "nop\nnop\nnop\nnop\nnop\n")

extern WiiController wiiController;

void caughtClock2() { /* noop */ }

// Handle full cycle from one latch until all 8 buttons have been sent
// or we have gone more than 12 nanoseconds without seeing another
// clock or latch.
void handleClatchCycle() {
    int loopsSinceAnyClock = 0;
    int clatchButtonsSent = 0;
    int clock2ButtonsSent = 0;

    // Immediately send the first button on a clock signal.
    if (wiiController.buttons[clatchButtonsSent]) {
        NES_SET_DATA1_LOW;
        NES_SET_DATA2_LOW;
    } else {
        NES_SET_DATA1_HIGH;
        NES_SET_DATA2_HIGH;
    }

    clatchButtonsSent++;
    clock2ButtonsSent++;

    // We want to see 8 clock cycles total, and we have already sent
    // our first button. So we poll until we see the interrupt register bit set
    // and this indicates that a clock cycle has occurred (we should send the
    // next button in anticipation.)
    while (clatchButtonsSent < 9 && clock2ButtonsSent < 9) {
        loopsSinceAnyClock++;

        // clock 1
        if (clatchButtonsSent < 9) {
            if (NES_CLATCH_INTERRUPT_IS_HIGH) { // clatch interrupt is high
                // Toggle interrupt handler to clear additional interrupts
                // that occurred during this ISR.
                NES_CLEAR_CLATCH_INTERRUPT;

                if (clatchButtonsSent == 8) {
                    // On our last cycle, we have already sent 8 buttons, so we
                    // should reset state and prepare to leave the ISR.
                    NES_SET_DATA1_LOW;
                } else if (wiiController.buttons[clatchButtonsSent]) {
                    NES_SET_DATA1_LOW;
                } else {
                    NES_SET_DATA1_HIGH;
                }

                clatchButtonsSent++;
                loopsSinceAnyClock = 0;
            } else {
                asm volatile ("nop\nnop\nnop\nnop\nnop\n");
                asm volatile ("nop\nnop\n");
            }
        } else {
                asm volatile ("nop\nnop\nnop\nnop\nnop\n");
                asm volatile ("nop\nnop\nnop\nnop\n");
        }

        // clock 2
        //if (clock2ButtonsSent < 9) {
        //    if (NES_CLOCK2_INTERRUPT_IS_HIGH) { // "interrupt" is high
        //        // Toggle interrupt handler to clear additional interrupts
        //        // that occurred during this ISR.
        //        NES_CLEAR_CLOCK2_INTERRUPT;

        //        if (clock2ButtonsSent == 8) {
        //            // On our last cycle, we have already sent 8 buttons, so we
        //            // should reset state and prepare to leave the ISR.
        //            //PORTD &= B10111111; // turns signal to low
        //        } else if (wiiController.buttons[clock2ButtonsSent]) {
        //            //PORTD &= B10111111; // turns signal to low
        //        } else {
        //            //PORTD |= B01000000; // turns signal to high
        //        }

        //        clock2ButtonsSent++;
        //        loopsSinceAnyClock = 0;
        //    }
        //}


        if (loopsSinceAnyClock > 12) {
            // We timed out because there were no clock cycles recently,
            // so we should reset state and prepare to leave the ISR.
            if (clatchButtonsSent == 8) {
                PORTD |= B01000000; // blue/white led to high
            }
            PORTD |= B00100000; // red led on
            break;
        }
    }

    NES_SET_DATA1_LOW;
    NES_SET_DATA2_LOW;

    // Toggle interrupt handler to clear additional interrupts
    // that occurred during this ISR.
    NES_CLEAR_CLATCH_INTERRUPT;
    NES_CLEAR_CLOCK2_INTERRUPT;
}

void NES::init() {
    CLATCH_PIN = 2;
    CLOCK2_PIN = 3;
    DATA_PIN = 4;

    attachInterrupt(0, handleClatchCycle, FALLING);
    attachInterrupt(1, caughtClock2, FALLING);

    // Setup clock latch and data pins for NES
    pinMode(CLOCK2_PIN, INPUT);
    pinMode(CLATCH_PIN, INPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);

    digitalWrite(DATA_PIN, LOW);
}

NES::NES() { }

NES::~NES() { }
