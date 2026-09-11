# ORBIT Wearable Monitoring - Early Prototype

ORBIT, or Onboard Readiness & Bio-Intelligence Telemetry, is an early-stage wearable biomedical monitoring project focused on using sensor-based data to support astronaut readiness and human performance monitoring concepts.

This repository contains the initial sEMG exercise-classification foundation for ORBIT. The current work uses MyoWare EMG voltage data to classify exercise movements and explore features that may support future muscle activation, fatigue, and readiness monitoring.

## Project Status

This is an early prototype and research foundation. The sEMG data collection and initial embedded classification workflow have been explored, while the full ORBIT wearable system is still in progress.

## Current Features

- MyoWare EMG data collection from an analog input.
- Exercise classes: squat, lunge, and jump.
- Windowed signal capture at approximately 100 Hz.
- Feature extraction using voltage mean, standard deviation, RMS, and peak-to-peak values.
- Embedded TensorFlow Lite Micro inference setup for Arduino-compatible hardware.
- Normalization constants and model header files for embedded deployment.

## Repository Structure

```text
arduino/
  data_collection/
    Data_Collection_ORBIT.ino
  classifier/
    ORBIT_AI_Gesture_Model.ino
    ORBIT_model.h
    ORBIT_normalizer.h
    ORBIT_gesture_model.tflite
notebooks/
  sEMG_feature_extraction.ipynb
data/
  squat_data.txt
  lunge_data.txt
  jump_data.txt
```

## Notebook Workflow

The notebook in `notebooks/` documents the sEMG feature extraction and model-development workflow used for the current prototype data. It is included so the data analysis process can be reviewed alongside the embedded Arduino deployment files.

## Hardware and Tools

- Arduino Nano 33 BLE Sense or similar Arduino-compatible board
- MyoWare 2.0 Muscle Sensor or equivalent sEMG module
- Surface EMG electrodes
- Arduino IDE
- TensorFlow Lite Micro
- Python-based model training workflow

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
- data processing: contains scripts used for data analysis and processing.
- data: contains datasets and collected project data.
- documentation: contains project documentation and reference materials.
- notebooks: contains Jupyter notebooks used for analysis and development.

Depending on which part of ORBIT you are working on, you may need:

- Git
- Python
- Jupyter Notebook or JupyterLab
- MATLAB
- Arduino IDE

The README file should be consulted for the current software versions required by the project.

Opening the project:

For Python:

Open Jupyter Notebook or JupyterLab.
Navigate to the ORBIT folder.
Open the notebooks or data processing folders as needed.

For MATLAB:

Open MATLAB.
Open the ORBIT project folder.
Add the project folders to the MATLAB path if required.

For Arduino:

Open the Arduino IDE.
Open the appropriate sketch from the arduino folder.
Verify that the correct board and COM port are selected.

## Notes

This project is for research, prototyping, and educational use. It is not intended for medical diagnosis or clinical decision-making.
