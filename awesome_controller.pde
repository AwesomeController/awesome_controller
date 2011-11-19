/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

int clock = 2;
int latch = 3;
int data = 4;

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(data, INPUT);
}

void loop() {

}
