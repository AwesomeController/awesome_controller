#include <SPI.h>
#include "ps3_usb.h"
#include "WiiController.h"
#include "BluetoothUsbHostHandler.h"

#define CONSOLE_NES  1
#define CONSOLE_SNES 2
#define CONSOLE_N64  3
#define CONSOLE_CHOICE CONSOLE_N64

int LATCH_PIN = 2;
int CLOCK_PIN = 3;
int DATA_PIN = 4;

volatile int buttonCyclesSinceLatch;
int buttonStatePrintCounter = 0;

// Dumb toggle so we can actually fake buttons
// These used to be 0x04 which I think is B00000100, but if something breaks, change these
#define SET_N64_PIN_HIGH DDRD &= ~B00000100
#define SET_N64_PIN_LOW DDRD |= B00000100
#define QUERY_N64_PIN (PIND & B00000100)
boolean oddN64ButtonCycle = true;
unsigned char N64RawCommandPacket[9]; // 1 received bit per byte

void handleN64CommandCycle();
void receiveN64CommandPacket();
void sendN64ButtonsResponse(unsigned char *buffer, char length);

PS3_USB PS3Game;
BluetoothUsbHostHandler bluetoothUsbHostHandler;
WiiController wiiController;

void setup()
{
    if (CONSOLE_CHOICE == CONSOLE_NES || CONSOLE_CHOICE == CONSOLE_SNES) {
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
    } else if (CONSOLE_CHOICE == CONSOLE_N64) {
        Serial.begin(9600);
        DATA_PIN = 2;
        digitalWrite(DATA_PIN, LOW);
        pinMode(DATA_PIN, INPUT);
        attachInterrupt(0, handleN64CommandCycle, FALLING); // Interrupt on Pin 2
    }
}

void initPS3Controller()
{
  Serial.println("PS3 USB library about to be initialized");
  PS3Game.init();
  Serial.println("PS3 USB library initialized");
}

void initBluetoothUsbHostHandler()
{
  Serial.println("Bluetooth USB Host Handler to be initialized");
  bluetoothUsbHostHandler.init();
  Serial.println("Bluetooth USB Host Handler initialized");
  bluetoothUsbHostHandler.setBDAddressMode(BD_ADDR_INQUIRY);
}

void loop()
{
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
    }
    if (handledInterrupt) {
      Serial.println("Handled interrupt");
      handledInterrupt = 0;
    }
}

void readButtons(void)
{
    wiiController.buttons[0] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_TWO);
    wiiController.buttons[1] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_ONE);
    wiiController.buttons[2] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_MINUS);
    wiiController.buttons[3] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_PLUS);
    wiiController.buttons[4] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_RIGHT);
    wiiController.buttons[5] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_LEFT);
    wiiController.buttons[6] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_UP);
    wiiController.buttons[7] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_DOWN);
    wiiController.buttons[8] = bluetoothUsbHostHandler.buttonPressed(WIIREMOTE_A);
}

void readControllerState()
{
//    PS3Game.task();
//    if ((PS3Game.statConnected()) && (PS3Game.statReportReceived())){ // report received ?
//        if (PS3Game.buttonChanged()){
//            buttons[0] = PS3Game.buttonPressed(buCross);
//            buttons[1] = PS3Game.buttonPressed(buSquare);
//            buttons[2] = PS3Game.buttonPressed(buSelect);
//            buttons[3] = PS3Game.buttonPressed(buStart);
//            buttons[4] = PS3Game.buttonPressed(buUp);
//            buttons[5] = PS3Game.buttonPressed(buDown);
//            buttons[6] = PS3Game.buttonPressed(buLeft);
//            buttons[7] = PS3Game.buttonPressed(buRight);
//            buttons[8] = PS3Game.buttonPressed(buCircle);
//            buttons[9] = PS3Game.buttonPressed(buTriangle);
//            buttons[10] = PS3Game.buttonPressed(buL1);
//            buttons[11] = PS3Game.buttonPressed(buR1);
//            buttons[12] = false;
//            buttons[13] = false;
//            buttons[14] = false;
//            buttons[15] = false;
//        }
//    }
}

// not needed at this moment (since NES ~= SNES) but might be a good starting point
// for having different controller types, etc.
//void readControllerStateNES()
//{
//    PS3Game.task();
//    if ((PS3Game.statConnected()) && (PS3Game.statReportReceived())){ // report received ?
//        if (PS3Game.buttonChanged()){
//            buttons[0] = PS3Game.buttonPressed(buCross);
//            buttons[1] = PS3Game.buttonPressed(buSquare);
//            buttons[2] = PS3Game.buttonPressed(buSelect);
//            buttons[3] = PS3Game.buttonPressed(buStart);
//            buttons[4] = PS3Game.buttonPressed(buUp);
//            buttons[5] = PS3Game.buttonPressed(buDown);
//            buttons[6] = PS3Game.buttonPressed(buLeft);
//            buttons[7] = PS3Game.buttonPressed(buRight);
//            buttons[8] = PS3Game.buttonPressed(buCircle);
//        }
//    }
//}

void snesKeyDown()
{
    if (wiiController.buttons[buttonCyclesSinceLatch] == 0) {
        PORTD |= B00010000; // turns signal to high
    } else {
        PORTD &= B11101111; // turns signal to low
    }
    buttonCyclesSinceLatch++;
}

void resetButtons()
{
    buttonCyclesSinceLatch = 0;
    snesKeyDown();
}

void handleN64CommandCycle()
{
    unsigned char command[] = { 0x90,0x00,0x00,0x00 }; // A + Start
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
    Serial.println("cycle over");
}

void receiveN64CommandPacket()
{
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

/**
 * This sends the given byte sequence to the controller.
 * Length must be at least 1.
 * Oh, it destroys the buffer passed in as it writes it. My B guys.
 */
void sendN64ButtonsResponse(unsigned char *buffer, char length) {
    // Send these bytes
    char bits;
    bool bit;

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
