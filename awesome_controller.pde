/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

int clock = 2;
int latch = 3;
int data = 4;
int led = 13;

int buttons[16];

void setup() {
  pinMode(led, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(data, INPUT);
  
  digitalWrite(clock, HIGH);
}

void loop() {
  digitalWrite(latch, HIGH);
  delayMicroseconds(12);
  digitalWrite(latch, LOW);
  delayMicroseconds(6);
  
  for (int i=0; i < 16; i++){
    digitalWrite(clock, LOW);
    buttons[i] = digitalRead(data);
    delayMicroseconds(6);
    
    digitalWrite(clock, HIGH);
    delayMicroseconds(6);
  }
  
  digitalWrite(led, buttons[4]);
  
  delay(17);
}
