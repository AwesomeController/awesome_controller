/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
//#include <avr/sfr_defs.h>
//#include <avr/io.h>
//#include <inttypes.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>
//#include "WConstants.h"

int latch = 2;
int clock = 3;
int data = 4;
int led = 13;

int state = HIGH;
int index = 0;

//Input is buttons[i]
int buttons[15];
int butttons[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};

//Output is on pin Data to snes

void setup() {
  attachInterrupt(0, resetButtons, RISING);
  attachInterrupt(1, snesKeyDown, RISING);
  //attachInterrupt(1, snesKeyUp, RISING);
  pinMode(led, OUTPUT);
  pinMode(clock, INPUT);
  pinMode(latch, INPUT);
  pinMode(data, OUTPUT);
  //Intialize states
  digitalWrite(data, state);
}

void loop() {
}

void snesKeyDown() {
  if (buttons[index] == 1) {
    PORTD |= B00010000;
  } else {
    PORTD &= B11101111;
  }
  index++;
}

void resetButtons() {
  index = 0;
}
