int ledpin = 13;

void setup() {
  pinMode(ledpin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(ledpin, HIGH);
  delay(5000);
  Serial.write("$$$");
}


void loop() {
  char val = 0;
  if (Serial.available()) {
    val = Serial.read();
  }

  if (val == 'C') {
    //digitalWrite(ledpin, LOW);
  }

  delay(100);
}
