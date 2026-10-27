#include "RPM.h"
#include "Config.h"

namespace {
volatile unsigned long pulseCount = 0;
volatile unsigned long lastEdgeUs = 0;

void onRpmRise() {
  const unsigned long now = micros();
  if (RPM_MIN_PULSE_US == 0 || (unsigned long)(now - lastEdgeUs) >= RPM_MIN_PULSE_US) {
    ++pulseCount;
    lastEdgeUs = now;
  }
}
}

void initRPM() {
  pinMode(RPM_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RPM_PIN), onRpmRise, RISING);
}

unsigned long takeRPMPulses() {
  noInterrupts();
  const unsigned long result = pulseCount;
  pulseCount = 0;
  interrupts();
  return result;
}

float calculateRPM(unsigned long pulses, unsigned long elapsedMs) {
  if (!elapsedMs || RPM_PULSES_PER_REVOLUTION <= 0) return 0.0f;
  const float hz = (pulses * 1000.0f) / elapsedMs;
  return hz * 60.0f / RPM_PULSES_PER_REVOLUTION;
}
