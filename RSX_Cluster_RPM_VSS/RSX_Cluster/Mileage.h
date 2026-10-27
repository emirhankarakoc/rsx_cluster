#pragma once
#include <stdint.h>
#include <limits.h>

// Hardware-independent distance model. Not yet connected to the UNO sketch.
// Persistence must save ALL fields, including pulse counts, without rounding.
class Mileage {
 public:
  struct State {
    uint64_t initialMilliMiles;
    uint64_t pulses;
    uint64_t tripAStart;
    uint64_t tripBStart;
    uint32_t pulsesPerMile;
  };

  // Commission with the documented OEM reading, e.g. 172450000 = 172450 mi.
  // A fresh/uncommissioned device must not silently start at zero.
  bool commission(uint64_t initialMilliMiles, uint32_t pulsesPerMile) {
    if (ready_ || !pulsesPerMile) return false;
    state_ = {initialMilliMiles, 0, 0, 0, pulsesPerMile};
    ready_ = true;
    return true;
  }

  // Storage adapter must verify record version and CRC BEFORE this.
  // A corrupt record must produce a fault, never automatic recommissioning.
  bool restore(const State& saved) {
    if (ready_ || !saved.pulsesPerMile || saved.tripAStart > saved.pulses ||
        saved.tripBStart > saved.pulses) return false;
    state_ = saved;
    ready_ = true;
    return true;
  }

  bool addPulses(uint64_t count) {
    if (!ready_ || count > UINT64_MAX - state_.pulses) return false;
    state_.pulses += count;
    return true;
  }

  bool resetTripA() {
    if (!ready_) return false;
    state_.tripAStart = state_.pulses;
    return true;
  }
  bool resetTripB() {
    if (!ready_) return false;
    state_.tripBStart = state_.pulses;
    return true;
  }
  bool ready() const { return ready_; }
  bool snapshot(State& output) const {
    if (!ready_) return false;
    output = state_;
    return true;
  }
  double totalMiles() const {
    return ready_ ? state_.initialMilliMiles / 1000.0 + miles(state_.pulses) : -1.0;
  }
  double tripAMiles() const {
    return ready_ ? miles(state_.pulses - state_.tripAStart) : -1.0;
  }
  double tripBMiles() const {
    return ready_ ? miles(state_.pulses - state_.tripBStart) : -1.0;
  }

 private:
  double miles(uint64_t pulses) const {
    return static_cast<double>(pulses) / state_.pulsesPerMile;
  }
  State state_ = {};
  bool ready_ = false;
};
