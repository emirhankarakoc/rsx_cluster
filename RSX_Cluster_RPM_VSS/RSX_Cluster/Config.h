#pragma once
#include <Arduino.h>

// Arduino UNO / Nano (ATmega328P): D2 and D3 support external interrupts.
constexpr uint8_t RPM_PIN = 2;
constexpr uint8_t VSS_PIN = 3;
constexpr unsigned long SAMPLE_INTERVAL_MS = 500;

// Initial RPM assumption from our earlier project; VERIFY against OBD/tach.
constexpr float RPM_PULSES_PER_REVOLUTION = 2.0f;

// VSS calibration: number of pulses per mile. Set based on GPS/OBD comparison.
// 0 means uncalibrated: VSS frequency is reported but speed is not calculated.
constexpr float VSS_PULSES_PER_MILE = 0.0f;

// Minimum spacing between counted rising edges to reject brief noise spikes.
// Tune to the actual maximum signal frequency; 0 disables filtering.
constexpr unsigned long RPM_MIN_PULSE_US = 100;
constexpr unsigned long VSS_MIN_PULSE_US = 100;

// Logic outputs to EXTERNAL protected lamp drivers, never cluster wires directly.
// HIGH enables a typical active-high transistor/driver input. Change individually
// to LOW for an inverting driver. These are Arduino pins, NOT RSX connector pins.
constexpr uint8_t CEL_OUTPUT_PIN = 4;
constexpr uint8_t CRUISE_OUTPUT_PIN = 5;
constexpr uint8_t SRS_OUTPUT_PIN = 6;
constexpr uint8_t LOWBEAM_OUTPUT_PIN = 7;
constexpr uint8_t HIGHBEAM_OUTPUT_PIN = 8;
constexpr uint8_t CEL_ON_LEVEL = HIGH;
constexpr uint8_t CRUISE_ON_LEVEL = HIGH;
constexpr uint8_t SRS_ON_LEVEL = HIGH;
constexpr uint8_t LOWBEAM_ON_LEVEL = HIGH;
constexpr uint8_t HIGHBEAM_ON_LEVEL = HIGH;
constexpr uint8_t LEFT_OUTPUT_PIN = 9;
constexpr uint8_t RIGHT_OUTPUT_PIN = 10;
constexpr uint8_t LEFT_ON_LEVEL = HIGH;
constexpr uint8_t RIGHT_ON_LEVEL = HIGH;
constexpr unsigned long TURN_HALF_PERIOD_MS = 500;
