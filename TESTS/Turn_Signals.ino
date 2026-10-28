//12v comes from a7 - left signal.
//12v comes from b9 - right signal
//cluster input - 24k - 24k - arduino input -24k -gnd

const byte LEFT_PIN = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LEFT_PIN, INPUT);
}

void loop() {
  bool leftOn = digitalRead(LEFT_PIN) == HIGH;

  Serial.print("{\"version\":1,\"left\":");
  Serial.print(leftOn ? "true" : "false");
  Serial.println("}");

  delay(50);
}
