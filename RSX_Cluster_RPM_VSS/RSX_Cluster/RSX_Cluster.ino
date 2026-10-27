#include "Config.h"
#include "RPM.h"
#include "VSS.h"
#include "Lights.h"

unsigned long lastSampleMs = 0;

void setup() {
  Serial.begin(115200);
  initLights();
  initRPM();
  initVSS();
  lastSampleMs = millis();
  Serial.println(F("RSX Cluster | RPM D2 | VSS D3"));
  Serial.println(F("# Lights D4-D10 ready, all OFF. Type HELP; use newline ending."));
  Serial.println(F("VSS speed uncalibrated until VSS_PULSES_PER_MILE is set."));
  Serial.println(F("elapsed_ms,rpm_pulses,rpm,vss_pulses,vss_hz,speed_mph"));
}

void loop() {
  serviceLightCommands();
  const unsigned long now = millis();
  const unsigned long elapsed = now - lastSampleMs;
  if (elapsed < SAMPLE_INTERVAL_MS) return;

  // Snapshot/reset each channel (slight timing skew is acceptable here).
  const unsigned long rpmPulses = takeRPMPulses();
  const unsigned long vssPulses = takeVSSPulses();
  lastSampleMs = now;

  const float rpm = calculateRPM(rpmPulses, elapsed);
  const float vssHz = calculateVSSHz(vssPulses, elapsed);
  const float mph = calculateSpeedMph(vssHz);

  Serial.print(elapsed);
  Serial.print(',');
  Serial.print(rpmPulses);
  Serial.print(',');
  Serial.print(rpm, 0);
  Serial.print(',');
  Serial.print(vssPulses);
  Serial.print(',');
  Serial.print(vssHz, 2);
  Serial.print(',');
  if (mph < 0) Serial.println(F("UNCALIBRATED"));
  else Serial.println(mph, 2);
}
