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

volatile int index = 0;
volatile int counter = 0;

//Input is buttons[i]
int buttons[] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
int buttons_feedback[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//Output is on pin Data to snes

void setup() {
  Serial.begin(9600);
  attachInterrupt(0, resetButtons, RISING);
  attachInterrupt(1, snesKeyDown, RISING);
  //attachInterrupt(1, snesKeyUp, RISING);
  pinMode(led, OUTPUT);
  pinMode(clock, INPUT);
  pinMode(latch, INPUT);
  pinMode(data, OUTPUT);
  //Intialize states
}

void loop() {
  if (counter > 64) {
      for (int i=0; i < 16; i++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.println(buttons_feedback[i]);
      }
    counter = 0;
  }
  
  if (32 == counter) {
    //cycling A
    buttons[7] = 0;
  }
  if (0 == counter) {
    buttons[7] = 1;
  }
}

void snesKeyDown() {
  //Serial.println("yay, I'm running!!");
  if (buttons[index] == 0) {
    PORTD |= B00010000; //turns signal to high
  } else {
    PORTD &= B11101111; //turns signal to low
  }
  buttons_feedback[index] = buttons[index];
  index++;
  //Serial.println(index);
}

void resetButtons() {
  index = 0;
  counter++;
  //Serial.println("Latch");
  
  //for (int i=0; i < 16; i++) {
  //Serial.print(i);
  //Serial.print(": ");
  //Serial.println(buttons_feedback[i]);
  //}
}
