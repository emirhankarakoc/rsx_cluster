//its not digital signal , its grounding when i press button. so intiial resistance is roughly 50kOhm. when i press cc button, it becomes 0.1k, 0.2k.
//input (a8) - 24k - 24k - arduino ANALOG input - 24k , gnd(b18)
//bu kod icin, a0 analog in kullanildi. 

const byte CRUISE_PIN = A0;

bool cruiseOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(CRUISE_PIN, INPUT_PULLUP);
}

void loop() {
  // Average 10 readings
  long total = 0;

  for (int i = 0; i < 10; i++) {
    total += analogRead(CRUISE_PIN);
    delay(2);
  }

  int raw = total / 10;

  // Active LOW with hysteresis
if (raw > 900) {
  cruiseOn = false;  // Disconnected
}
else if (raw < 345) {
  cruiseOn = true;
}
else if (raw > 365) {
  cruiseOn = false;
}

  Serial.print("{\"version\":1,\"cruise\":");
  Serial.print(cruiseOn ? "true" : "false");

  Serial.print(",\"raw\":");
  Serial.print(raw);

  Serial.println("}");

  delay(80);
}
