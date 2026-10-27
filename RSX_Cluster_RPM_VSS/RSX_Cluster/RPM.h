#pragma once
#include <Arduino.h>
void initRPM();
unsigned long takeRPMPulses();
float calculateRPM(unsigned long pulses, unsigned long elapsedMs);
