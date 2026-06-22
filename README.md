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

## Notes

This project is for research, prototyping, and educational use. It is not intended for medical diagnosis or clinical decision-making.
