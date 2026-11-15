# Onboard Readiness & Bio-Intelligence Telemetry (ORBIT)

ORBIT, or Onboard Readiness & Bio-Intelligence Telemetry, is an early-stage wearable biomedical monitoring project focused on using sensor-based data to support astronaut readiness and human performance monitoring concepts.

This repository contains the initial sEMG exercise-classification foundation for ORBIT. The current work uses MyoWare EMG voltage data to classify exercise movements and explore features that may support future muscle activation, fatigue, and readiness monitoring.

## Project Status

This is an early prototype and research foundation. The sEMG data collection and initial embedded classification workflow have been explored, while the full ORBIT wearable system is still in progress.

## Current Features

- MyoWare EMG data collection from an analog input.
- Send collected analog signals to a Raspberry Pi.
- Windowed signal capture at approximately 100 Hz.
- Feature extraction using voltage mean, standard deviation, and peak-to-peak values.
- Calculate root-mean square (RMS) and median density frequency (MDF) values.


## Notebook Workflow

The notebook in `notebooks/` documents the sEMG feature extraction and model-development workflow used for the current prototype data. It is included so the data analysis process can be reviewed alongside the embedded Arduino deployment files.

## Hardware and Tools

- Arduino Nano 33 BLE Sense or similar Arduino-compatible board
- MyoWare 2.0 Muscle Sensor or equivalent sEMG module
- Surface EMG electrodes
- Arduino IDE
- Python-based model training workflow
- Raspberry Pi

## How to:

Downloading Repository

- First, open a terminal or command prompt.
- Clone the repository:
  git clone <ORBIT-repository-url>
- After the download completes, enter the project folder: cd ORBIT

What the repository contains:
- Dashboard: Displays and computer setup
- Raspberry Pi: contains Raspberry Pi code and related files.
- Arduino: contains Arduino firmware and microcontroller programs.
- data: contains datasets and collected project data.

The repository also includes:

Depending on which part of ORBIT you are working on, you may need:

- Git
- Python
- VS Code
- Arduino IDE

The README file should be consulted for the current software versions required by the project.
