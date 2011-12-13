#include <SPI.h>
#include <Arduino.h>
#include "n64.h"

boolean oddN64ButtonCycle = true;
unsigned char N64RawCommandPacket[9]; // 1 received bit per byte

/**
 * This sends the given byte sequence to the controller.
 * Length must be at least 1.
 * Oh, it destroys the buffer passed in as it writes it. My B guys.
 */
void sendN64ButtonsResponse(unsigned char *buffer, char length) {
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

void receiveN64CommandPacket() {
    // listen for the expected 8 bytes of data back from the controller and
    // store it in N64RawCommandPacket, one bit per byte
    asm volatile (";Starting to listen");
    unsigned char timeout1;
    unsigned char timeout2;
    char bitcount = 9;  // should be 9 for 9 bits
    unsigned char *bitbin = N64RawCommandPacket;

    // Again, using gotos here to make the assembly more predictable and
    // optimization easier (please don't kill me)
read_loop:
    timeout1 = 0xff;
    // wait for line to go low
    while (QUERY_N64_PIN) {
        // Totally safe infinite while loop
        // Trust us. We're doctors.
        // Won't send your arduino in an infinite loop if you unplug the controller. 100 True statement.
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
    *bitbin = QUERY_N64_PIN;
    ++bitbin;
    --bitcount;
    if (bitcount == 0) {
        return;
    }

    // wait for line to go high again
    // it may already be high, so this should just drop through
    timeout2 = 0x3f;
    while (!QUERY_N64_PIN) {
        // Totally safe infinite while loop
        // Trust us. We're doctors.
        // Won't send your arduino in an infinite loop if you unplug the controller. 100 True statement.
    }
    goto read_loop;

}

void handleN64CommandCycle() {
    unsigned char command[] = { 0x90, 0x00, 0x00, 0x00 }; // A + Start
    if (oddN64ButtonCycle == 0) {
        command[0] = 0x00; // None pressed
    } else {
        command[0] = 0x80; // Start pressed
    }
    receiveN64CommandPacket();
    // send those 3 bytes
    sendN64ButtonsResponse(command, 4);
    // end of time sensitive code
    oddN64ButtonCycle = !oddN64ButtonCycle;
}

N64::N64() {
}

N64::~N64() {
}

void N64::init() {
    oddN64ButtonCycle = true;
    DATA_PIN = 2;
    pinMode(DATA_PIN, INPUT);
    digitalWrite(DATA_PIN, LOW);
    attachInterrupt(0, handleN64CommandCycle, FALLING); // Interrupt on Pin 2
}
