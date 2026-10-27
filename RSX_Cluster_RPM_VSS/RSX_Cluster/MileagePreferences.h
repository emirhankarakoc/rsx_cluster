#pragma once
#if defined(ARDUINO_ARCH_ESP32)
#include <Preferences.h>
#include <string.h>
#include "Mileage.h"

// Call from one loop/task only, never from a VSS ISR. No GPIO dependencies.
class MileagePreferences {
 public:
  enum class Boot { Loaded, Uncommissioned, Fault };

  Boot begin() {
    storageFault_ = true;
    if (opened_) return Boot::Fault;
    if (!prefs_.begin("rsx-mileage", false)) return Boot::Fault;
    opened_ = true;
    if (!prefs_.isKey("state")) {
      empty_ = true;
      storageFault_ = false;
      return Boot::Uncommissioned;
    }
    uint8_t data[44];
    if (prefs_.getBytesLength("state") != sizeof(data) ||
        prefs_.getBytes("state", data, sizeof(data)) != sizeof(data) ||
        get(data, 4) != 0x31585352UL ||
        get(data + 40, 4) != crc(data, 40)) return Boot::Fault;
    Mileage::State state = {get(data + 4, 8), get(data + 12, 8),
                           get(data + 20, 8), get(data + 28, 8),
                           static_cast<uint32_t>(get(data + 36, 4))};
    if (!model_.restore(state)) return Boot::Fault;
    savedTick_ = ticks(state.pulses, state.pulsesPerMile);
    storageFault_ = false;
    return Boot::Loaded;
  }

  // Installation tool only. Never expose through the ordinary dashboard UI.
  // A failed initial write leaves the model ready, but storageFault() true.
  bool commission(uint64_t oemMilliMiles, uint32_t pulsesPerMile) {
    if (!opened_ || !empty_ || !model_.commission(oemMilliMiles, pulsesPerMile))
      return false;
    empty_ = false;
    dirty_ = true;
    return save();
  }

  bool addPulses(uint64_t pulses) { return model_.addPulses(pulses); }
  const Mileage& value() const { return model_; }
  bool storageFault() const { return storageFault_; }

  // Invoke each loop AFTER adding newly counted VSS pulses. Failed writes retry
  // at most once/second. Use the returned fault status in the dashboard.
  bool service(uint32_t nowMs) {
    Mileage::State state;
    if (!model_.snapshot(state)) return false;
    const bool due = dirty_ || ticks(state.pulses, state.pulsesPerMile) != savedTick_;
    if (!due) return !storageFault_;
    if (storageFault_ && static_cast<uint32_t>(nowMs - lastAttemptMs_) < 1000)
      return false;
    lastAttemptMs_ = nowMs;
    return save();
  }

  // False means reset is pending in RAM, NOT durably acknowledged. service()
  // retries it. Callers must show a storage fault rather than report success.
  bool resetTripA() {
    if (!model_.resetTripA()) return false;
    dirty_ = true;
    return save();
  }
  bool resetTripB() {
    if (!model_.resetTripB()) return false;
    dirty_ = true;
    return save();
  }

 private:
  static uint64_t ticks(uint64_t pulses, uint32_t ppm) {
    // Compare 0.1-mile boundaries without rounding the saved pulse history.
    // Division first avoids multiplying a potentially large pulse counter.
    return pulses / ppm * 10 + (pulses % ppm) * 10 / ppm;
  }
  static void put(uint8_t* out, uint64_t v, uint8_t bytes) {
    for (uint8_t i = 0; i < bytes; ++i) { out[i] = v & 255; v >>= 8; }
  }
  static uint64_t get(const uint8_t* in, uint8_t bytes) {
    uint64_t v = 0;
    for (uint8_t i = 0; i < bytes; ++i) v |= uint64_t(in[i]) << (8 * i);
    return v;
  }
  static uint32_t crc(const uint8_t* data, uint8_t size) {
    uint32_t v = 0xffffffffUL;
    for (uint8_t i = 0; i < size; ++i) {
      v ^= data[i];
      for (uint8_t bit = 0; bit < 8; ++bit)
        v = (v >> 1) ^ (0xedb88320UL & (0UL - (v & 1)));
    }
    return ~v;
  }
  bool save() {
    Mileage::State state;
    if (!opened_ || !model_.snapshot(state)) return false;
    uint8_t data[44] = {};
    put(data, 0x31585352UL, 4); // RSX1 record version
    put(data + 4, state.initialMilliMiles, 8);
    put(data + 12, state.pulses, 8);
    put(data + 20, state.tripAStart, 8);
    put(data + 28, state.tripBStart, 8);
    put(data + 36, state.pulsesPerMile, 4);
    put(data + 40, crc(data, 40), 4);
    uint8_t verify[44];
    storageFault_ = prefs_.putBytes("state", data, sizeof(data)) != sizeof(data) ||
      prefs_.getBytes("state", verify, sizeof(verify)) != sizeof(verify) ||
      memcmp(data, verify, sizeof(data)) != 0;
    if (storageFault_) return false;
    savedTick_ = ticks(state.pulses, state.pulsesPerMile);
    dirty_ = false;
    return true;
  }
  Preferences prefs_;
  Mileage model_;
  bool opened_ = false;
  bool empty_ = false;
  bool dirty_ = false;
  bool storageFault_ = false;
  uint64_t savedTick_ = 0;
  uint32_t lastAttemptMs_ = 0;
};
#endif
