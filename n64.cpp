#include <SPI.h>
#include <Arduino.h>
#include "n64.h"
#include "WiiController.h"

extern WiiController wiiController;
unsigned char N64RawCommandPacket[9]; // 1 received bit per byte
volatile bool oddButton = false;
volatile unsigned char command[] = { 0x80, 0x00, 0x00, 0x00 };
volatile int counter = 0;

/**
 * This sends the given byte sequence to the controller.
 * Length must be at least 1.
 * Oh, it destroys the buffer passed in as it writes it. My B guys.
 */
void sendN64ButtonsResponse(volatile unsigned char *buffer, char length) {

    // Send these bytes
    char bits;

    // This routine is very carefully timed by examining the assembly output.
    // Do not change any statements, it could throw the timings off
    //
    // We get 16 cycles per microsecond, which should be plenty, but we need to
    // be conservative. Most assembly ops take 1 cycle, but a few take 2
    //
    // I use manually constructed for-loops out of gotos so I have more control
    // over the outputted assembly. I can insert nops where it was impossible
    // with a for loop

    asm volatile (";Starting outer for loop");
outer_loop:
    {
        asm volatile (";Starting inner for loop");
        bits=8;
inner_loop:
        {
            // Starting a bit, set the line low
            asm volatile (";Setting line to low");
            SET_N64_PIN_LOW; // 1 op, 2 cycles

            asm volatile (";branching");
            if (*buffer >> 7) {
                asm volatile (";Bit is a 1");
                // 1 bit
                // remain low for 1us, then go high for 3us
                // nop block 1
                asm volatile ("nop\nnop\nnop\nnop\nnop\n");

                asm volatile (";Setting line to high");
                SET_N64_PIN_HIGH;

                // nop block 2
                // we'll wait only 2us to sync up with both conditions
                // at the bottom of the if statement
                asm volatile ("nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              );

            } else {
                asm volatile (";Bit is a 0");
                // 0 bit
                // remain low for 3us, then go high for 1us
                // nop block 3
                asm volatile ("nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\nnop\n"
                              "nop\n");

                asm volatile (";Setting line to high");
                SET_N64_PIN_HIGH;

                // wait for 1us
                asm volatile ("; end of conditional branch, need to wait 1us more before next bit");
            }

            // end of the if, the line is high and needs to remain
            // high for exactly 16 more cycles, regardless of the previous
            // branch path

            asm volatile (";finishing inner loop body");
            --bits;
            if (bits != 0) {
                // nop block 4
                // this block is why a for loop was impossible
                asm volatile ("nop\nnop\nnop\nnop\nnop\n"
                              "nop\nnop\nnop\nnop\n");
                // rotate bits
                asm volatile (";rotating out bits");
                *buffer <<= 1;

                goto inner_loop;
            } // fall out of inner loop
        }
        asm volatile (";continuing outer loop");
        // In this case: the inner loop exits and the outer loop iterates,
        // there are /exactly/ 16 cycles taken up by the necessary operations.
        // So no nops are needed here (that was lucky!)
        --length;
        if (length != 0) {
            ++buffer;
            goto outer_loop;
        } // fall out of outer loop
    }

    // send a single stop (1) bit
    // nop block 5
    asm volatile ("nop\nnop\nnop\nnop\n");
    SET_N64_PIN_LOW;

    // wait 1 us, 16 cycles, then raise the line
    // 16-2=14
    // nop block 6
    asm volatile ("nop\nnop\nnop\nnop\nnop\n"
                  "nop\nnop\nnop\nnop\nnop\n"
                  "nop\nnop\nnop\nnop\n");
    SET_N64_PIN_HIGH;
}

bool receiveN64CommandPacket() {
    // listen for the expected 8 bytes of data back from the controller and
    // store it in N64RawCommandPacket, one bit per byte
    asm volatile (";Starting to listen");
//I AM HERE
    unsigned char timeout1;
    unsigned char timeout2;
    char bitsToRead = 9;
    unsigned char *commandPacket = N64RawCommandPacket;

    // Again, using gotos here to make the assembly more predictable and
    // optimization easier (please don't kill me)
read_loop:
    timeout1 = 0xff;

    // wait for line to go low
    // otherwise, wait a little and then return
    while (QUERY_N64_PIN) {
        //timeout1--;
//        if (!--timeout1) {
//            // not ready to start loop if we didn't get a low signal
//            return false;
//        }
    }

    // wait approx 2us and poll the line
    asm volatile (
                  "nop\nnop\nnop\nnop\nnop\n"
                  "nop\nnop\nnop\nnop\nnop\n"
                  "nop\nnop\nnop\nnop\nnop\n"
                  "nop\nnop\nnop\nnop\nnop\n"
                  "nop\nnop\nnop\nnop\nnop\n"
                  "nop\nnop\nnop\nnop\nnop\n"
            );

    // We found another bit, so we should decrease the amount we
    // are looking for. If we found them all, great, our array is
    // fully populated.
    *commandPacket = QUERY_N64_PIN;
    ++commandPacket;
    --bitsToRead;
    if (bitsToRead == 0) {  // counting down from 9 (number of bits)
        return true;
    }

    // wait for line to go high again
    // it may already be high, so this should just drop through
    timeout2 = 0x3f;

    while (!QUERY_N64_PIN) {
//        timeout2--;
//        if (!--timeout2) {
//            // not ready to end loop if we didn't get a high signal
//            return false;
//        }
    }

    goto read_loop;
}

void handleN64CommandCycle() {
    oddButton = !oddButton;
    if (oddButton) {
        return;
    }
    //if (wiiController.buttons[0] == 1) {
    //    command[0] = 0x80; // Start pressed
    //} else {
    //    command[0] = 0x00; // None pressed
    //}

    // begin time sensitive code
    if (receiveN64CommandPacket()) {
        sendN64ButtonsResponse(command, 4);
    }
    // end of time sensitive code

    if (wiiController.buttons[3]) {
        command[0] = 0x80;
        PORTD |= B00001000; // canary led
    } else {
        command[0] = 0x00;
        PORTD &= B11110111; // canary led
    }
    command[1] = 0x00;
    command[2] = 0x00;
    command[3] = 0x00;
}

N64::N64() {
}

N64::~N64() {
}

void N64::init() {
    DATA_PIN = 2;
    pinMode(DATA_PIN, INPUT);
    digitalWrite(DATA_PIN, LOW);

    // debugging LED
    DDRD  |= B00010000;
    PORTD &= B11101111;

    // canary LED
    DDRD  |= B00001000;
    //PORTD |= B00001000;

    attachInterrupt(0, handleN64CommandCycle, FALLING); // Interrupt on Pin 2
}
