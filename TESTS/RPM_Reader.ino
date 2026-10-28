//sadece rpm okur
//b14(input) - 24k - 24k - arduino input - 24k - ground(b18)
const byte RPM_PIN = 2;

volatile unsigned long pulses = 0;
volatile unsigned long lastPulse = 0;

const float PULSES_PER_REV = 2.0;

void rpmInterrupt() {
  unsigned long now = micros();

  if (now - lastPulse >= 100) {
    pulses++;
    lastPulse = now;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RPM_PIN, INPUT);

  attachInterrupt(
    digitalPinToInterrupt(RPM_PIN),
    rpmInterrupt,
    RISING
  );
}

void loop() {
  static unsigned long lastTime = millis();

  unsigned long now = millis();

  if (now - lastTime >= 500) {
    unsigned long elapsed = now - lastTime;
    lastTime = now;

    noInterrupts();
    unsigned long count = pulses;
    pulses = 0;
    interrupts();

    float hz = count * 1000.0 / elapsed;
    float rpm = hz * 60.0 / PULSES_PER_REV;

    Serial.print("{\"version\":1,\"rpm\":");
    Serial.print(rpm, 0);
    Serial.println("}");
  }
}
