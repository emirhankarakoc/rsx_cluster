
# 2002 Acura RSX — Digital Cluster

Custom digital instrument cluster project using Arduino, C++ and Python/Qt.

Reading signals directly from the original cluster harness.

## Signals & Pins

| Signal | RSX Cluster Pin |
|---|---|
| RPM | B14 |
| Speed (VSS) | B15 |
| Check Engine Light (CEL) | A22 |
| Left Turn Signal | A7 |
| Right Turn Signal | B9 |
| Cruise Control | A8 |

Arduino reads the signals and sends data over USB serial port (115200 baud). You also can see wiring diagram for 2002 rsx in files. I added as pdf files.

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

- ESP32 + K-Line communication
- Read ECT, IAT, STFT and LTFT from ECU
- Integrate the 5-inch display
- Build a standalone digital cluster
- THEN HEAD UNIT ->>THEN ECU.

**Status:** Work in progress: im having problems with soldering on protoboard. We are using voltage divider and each input brings 5 things , so breadbord becomes a hell. I need a protoboard for keep this project organized...but having problems on soldering. (20 nov)
