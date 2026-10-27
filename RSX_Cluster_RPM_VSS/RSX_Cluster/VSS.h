#pragma once
#include <Arduino.h>
void initVSS();
unsigned long takeVSSPulses();
float calculateVSSHz(unsigned long pulses, unsigned long elapsedMs);
float calculateSpeedMph(float vssHz);
