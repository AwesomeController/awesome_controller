int ledpin = 13;

void setup() {
  pinMode(ledpin, OUTPUT);
  Serial.begin(115200);
  digitalWrite(ledpin, HIGH);
  delay(5000);
  Serial.write("$$$");
  Serial.write("I\n");
}

void loop() {
  char val = 0;
  if (Serial.available()) {
    val = Serial.read();
  }

  if (val == ',') {
    digitalWrite(ledpin, LOW);
  }

  delay(100);
}
