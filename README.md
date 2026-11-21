
# 2002 Acura RSX BASE AUTO — Digital Cluster

Custom digital instrument cluster project using Arduino, C++, and Python/Qt.

Reading signals directly from the original cluster wiring.

## Signals & Pins

| Signal | RSX Cluster Pin |
|---|---|
| RPM | B14 |
| Speed (VSS) | B15 |
| Check Engine Light (CEL) | A22 |
| Left Turn Signal | A7 |
| Right Turn Signal | B9 |
| Cruise Control | A8 |

The `TESTS/` folder contains individual Arduino sketches for each signal. I've tested these inputs separately, and they're working. The next step is combining them into one system.

Arduino reads the signals and sends data over USB serial (115200 baud).

I've also included the 2002 RSX cluster wiring diagrams as PDF files in this repository.

## Dashboard UI

The digital dashboard is located in [`Cluster_UI/`](Cluster_UI/).

Run with Python 3.11+:

```bash
cd Cluster_UI
python -m pip install -r requirements.txt
python main.py
```

The UI supports demo mode without connecting hardware.

## Videos

- [Video 1](https://youtu.be/6FpHpLWUcVM)
- [Video 2](https://youtu.be/DHChB6jY_es)

## Next Steps

- Combine all tested signals into one system
- ESP32 + K-Line communication
- Read ECT, IAT, STFT, and LTFT from the ECU
- Integrate the 5-inch display
- Build a standalone digital cluster
- Future projects: Custom head unit → ECU

## Current Status — Nov 20, 2026

Currently working on the hardware. I'm using voltage dividers to read the signals, but the breadboard is getting messy with all the wiring.

I'm trying to move everything onto a protoboard, although soldering has been a bit challenging.
