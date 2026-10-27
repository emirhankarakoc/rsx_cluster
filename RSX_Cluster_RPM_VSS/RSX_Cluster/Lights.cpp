#include <Arduino.h>
#include <string.h>
#include "Config.h"
#include "Lights.h"

namespace {
struct Light {
  const char* name;
  uint8_t pin;
  uint8_t onLevel;
  bool enabled;
  bool blinking;
};

Light lights[] = {
  {"CEL", CEL_OUTPUT_PIN, CEL_ON_LEVEL, false},
  {"CRUISE", CRUISE_OUTPUT_PIN, CRUISE_ON_LEVEL, false},
  {"SRS", SRS_OUTPUT_PIN, SRS_ON_LEVEL, false},
  {"LOWBEAM", LOWBEAM_OUTPUT_PIN, LOWBEAM_ON_LEVEL, false},
  {"HIGHBEAM", HIGHBEAM_OUTPUT_PIN, HIGHBEAM_ON_LEVEL, false},
  {"LEFT", LEFT_OUTPUT_PIN, LEFT_ON_LEVEL, false},
  {"RIGHT", RIGHT_OUTPUT_PIN, RIGHT_ON_LEVEL, false}
};
char command[48];
uint8_t used = 0;
bool overflow = false;
unsigned long lastBlinkMs = 0;
bool blinkPhase = true;

void setLight(Light& light, bool enabled) {
  digitalWrite(light.pin, enabled ? light.onLevel :
               (light.onLevel == HIGH ? LOW : HIGH));
  light.enabled = enabled;
}

void printStatus() {
  Serial.print(F("# LIGHTS"));
  for (const auto& light : lights) {
    Serial.print(' ');
    Serial.print(light.name);
    Serial.print('=');
    Serial.print(light.enabled ? 1 : 0);
    if (light.blinking) Serial.print(F("(BLINK)"));
  }
  Serial.println();
}

void executeCommand() {
  char* name = strtok(command, " \t");
  if (!name) return;
  char* value = strtok(nullptr, " \t");
  char* extra = strtok(nullptr, " \t");
  if (strcmp(name, "HELP") == 0 && !value) {
    Serial.println(F("# CEL|CRUISE|SRS|LOWBEAM|HIGHBEAM|LEFT|RIGHT|ALL ON|OFF (or 1|0)"));
    Serial.println(F("# LEFT|RIGHT BLINK; HAZARD ON|OFF; LIGHTS"));
    return;
  }
  if (strcmp(name, "LIGHTS") == 0 && !value) {
    printStatus();
    return;
  }
  const bool isTurn = !strcmp(name, "LEFT") || !strcmp(name, "RIGHT");
  const bool hazard = !strcmp(name, "HAZARD");
  if (value && !extra && ((isTurn && !strcmp(value, "BLINK")) ||
      (hazard && (!strcmp(value, "ON") || !strcmp(value, "OFF") ||
                  !strcmp(value, "1") || !strcmp(value, "0"))))) {
    const bool blinking = strcmp(value, "OFF") && strcmp(value, "0");
    lastBlinkMs = millis();
    blinkPhase = true;
    for (auto& light : lights) {
      if ((!strcmp(light.name, "LEFT") || !strcmp(light.name, "RIGHT")) &&
          (hazard || !strcmp(name, light.name))) {
        light.blinking = blinking;
        setLight(light, blinking);
      }
      // Keep all blinking outputs in phase when a second turn is enabled.
      if (light.blinking) setLight(light, true);
    }
    printStatus();
    return;
  }
  if (!value || extra ||
      (strcmp(value, "ON") && strcmp(value, "OFF") &&
       strcmp(value, "1") && strcmp(value, "0"))) {
    Serial.println(F("# ERROR: use HELP"));
    return;
  }
  const bool enabled = !strcmp(value, "ON") || !strcmp(value, "1");
  bool found = false;
  for (auto& light : lights) {
    if (!strcmp(name, light.name) || !strcmp(name, "ALL")) {
      light.blinking = false;
      setLight(light, enabled);
      found = true;
    }
  }
  if (found) printStatus();
  else Serial.println(F("# ERROR: unknown light; use HELP"));
}
}

void initLights() {
  for (auto& light : lights) {
    // Preload the OFF output latch before switching the pin to output.
    setLight(light, false);
    pinMode(light.pin, OUTPUT);
  }
}

void serviceLightCommands() {
  const unsigned long now = millis();
  if (now - lastBlinkMs >= TURN_HALF_PERIOD_MS) {
    lastBlinkMs = now;
    blinkPhase = !blinkPhase;
    for (auto& light : lights) {
      if (light.blinking) setLight(light, blinkPhase);
    }
  }
  // Bounded work per loop so incoming serial traffic cannot starve sampling.
  for (uint8_t budget = 0; budget < 32 && Serial.available(); ++budget) {
    char c = Serial.read();
    if (c == '\r' || c == '\n') {
      if (overflow) Serial.println(F("# ERROR: command too long"));
      else {
        command[used] = '\0';
        executeCommand();
      }
      used = 0;
      overflow = false;
    } else if (!overflow) {
      if (used >= sizeof(command) - 1) overflow = true;
      else {
        if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
        command[used++] = c;
      }
    }
  }
}
