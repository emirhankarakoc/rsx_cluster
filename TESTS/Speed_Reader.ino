// sadece hizi okur
// input - paralel olarak 2 tane 24k veya tek 12k - arduino input - gnd
const byte VSS_PIN = 2;

const float PULSES_PER_MILE = 4000.0;
const unsigned long UPDATE_MS = 50;
const unsigned long MIN_PULSE_US = 5000;
const unsigned long RESET_GAP_US = 1500000;

volatile unsigned long lastPulseUs = 0;
volatile unsigned long periods[3] = {0, 0, 0};
volatile byte periodIndex = 0;
volatile byte periodCount = 0;

void onSpeedPulse() {
  unsigned long now = micros();

  if (lastPulseUs != 0) {
    unsigned long delta = now - lastPulseUs;

    if (delta < MIN_PULSE_US) return;

    if (delta > RESET_GAP_US) {
      periodIndex = 0;
      periodCount = 0;
    } else {
      periods[periodIndex] = delta;
      periodIndex = (periodIndex + 1) % 3;

      if (periodCount < 3) periodCount++;
    }
  }

  lastPulseUs = now;
}

void setup() {
  Serial.begin(115200);
  pinMode(VSS_PIN, INPUT_PULLUP);

  attachInterrupt(
    digitalPinToInterrupt(VSS_PIN),
    onSpeedPulse,
    RISING
  );
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long nowMs = millis();

  if (nowMs - lastUpdate < UPDATE_MS) return;
  lastUpdate = nowMs;

  unsigned long last;
  unsigned long sum = 0;
  byte count;

  noInterrupts();

  last = lastPulseUs;
  count = periodCount;

  for (byte i = 0; i < count; i++) {
    sum += periods[i];
  }

  interrupts();

  float hz = 0.0;

  if (last != 0 && count > 0) {
    unsigned long ageUs = micros() - last;
    float avgPeriod = (float)sum / count;

    float effectivePeriod =
      max(avgPeriod, (float)ageUs);

    unsigned long timeoutUs = constrain(
      (unsigned long)(avgPeriod * 2.5),
      350000UL,
      1500000UL
    );

    if (ageUs < timeoutUs) {
      hz = 1000000.0 / effectivePeriod;
    }
  }

  float mph = hz * 3600.0 / PULSES_PER_MILE;

  if (mph < 0.1) mph = 0.0;

  Serial.print("{\"version\":1,\"speed_mph\":");
  Serial.print(mph, 2);
  Serial.println("}");
}
