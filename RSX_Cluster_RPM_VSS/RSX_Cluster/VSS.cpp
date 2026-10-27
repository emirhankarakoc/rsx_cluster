#include "VSS.h"
#include "Config.h"

namespace {
volatile unsigned long pulseCount = 0;
volatile unsigned long lastEdgeUs = 0;

void onVssRise() {
  const unsigned long now = micros();
  if (VSS_MIN_PULSE_US == 0 || (unsigned long)(now - lastEdgeUs) >= VSS_MIN_PULSE_US) {
    ++pulseCount;
    lastEdgeUs = now;
  }
}
}

void initVSS() {
  pinMode(VSS_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(VSS_PIN), onVssRise, RISING);
}

unsigned long takeVSSPulses() {
  noInterrupts();
  const unsigned long result = pulseCount;
  pulseCount = 0;
  interrupts();
  return result;
}

float calculateVSSHz(unsigned long pulses, unsigned long elapsedMs) {
  if (!elapsedMs) return 0.0f;
  return (pulses * 1000.0f) / elapsedMs;
}

float calculateSpeedMph(float vssHz) {
  if (VSS_PULSES_PER_MILE <= 0.0f) return -1.0f;
  return vssHz * 3600.0f / VSS_PULSES_PER_MILE;
}
