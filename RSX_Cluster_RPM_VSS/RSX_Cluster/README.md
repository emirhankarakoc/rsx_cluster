# RSX RPM + VSS Reader (Arduino UNO/Nano)

For the planned ESP32-S3 replacement dashboard, see [architecture and status](ESP32_PLAN_TR.md).
`Mileage.h` provides an independent pulse-based distance model; it is not yet
integrated into this sketch. `MileagePreferences.h` adds ESP32-only Preferences/NVS
persistence at 0.1-mile boundaries and on trip resets; hardware testing is pending.

Now includes seven external-driver logic outputs for CEL, cruise, SRS,
low-beam request, high beam, left turn and right turn, with serial commands
and nonblocking turn/hazard blinking. See [Turkish wiring and commands](WIRING_TR.md).
RSX connector pins are pending documentation verification; Arduino pins are
assigned, but this project does not yet provide a verified vehicle pinout.

## Files
- `RSX_Cluster.ino`: serial reporting at 115200 baud every 500 ms.
- `RPM.h` / `RPM.cpp`: interrupt-based RPM pulse counting on D2.
- `VSS.h` / `VSS.cpp`: interrupt-based vehicle-speed pulse counting on D3.
- `Config.h`: pins, timing, pulse parameters and VSS calibration.
- `Lights.h` / `Lights.cpp`: D4–D10 driver outputs and line-based serial commands.

## Getting started
1. Unzip and open `RSX_Cluster/RSX_Cluster.ino` in Arduino IDE.
2. Choose your **Arduino Uno** (or compatible Nano) and correct serial port.
3. Upload. Open Serial Monitor at **115200 baud**.
4. Output is CSV: `elapsed_ms,rpm_pulses,rpm,vss_pulses,vss_hz,speed_mph`.
5. `RPM_PULSES_PER_REVOLUTION = 2.0` is the previous project assumption, **not a confirmed RSX ECU specification**. Compare with a trusted tachometer/OBD reading.
6. `VSS_PULSES_PER_MILE = 0` deliberately disables mph calculations pending calibration. Raw pulses and Hz still work. After obtaining a known steady speed and Hz, compute `pulses per mile = Hz * 3600 / mph` and enter it in `Config.h`. Repeat tests at several speeds, ideally closed course/dyno, to verify.

## Wiring safety
**Do not connect an unknown vehicle ECU/cluster wire straight to an Arduino input.** Signal voltages and transients may exceed 5V or be negative. Confirm signal identity, waveform, grounds and levels from the vehicle's wiring documentation and a scope/multimeter. Use an appropriately designed, automotive-rated, protected signal-conditioning interface (e.g. opto-isolation or comparator/level conditioning, as required) before D2/D3. Ensure safe power supply, fused wiring, and protected grounds. The sketch uses `INPUT`, expecting an externally conditioned 0–5V digital signal with a defined idle state. Do not connect to CAN-H/CAN-L directly; this sketch reads conditioned pulse inputs, **not CAN/OBD-II**.

Test first with a signal generator; don't experiment with wiring or displays while driving.
