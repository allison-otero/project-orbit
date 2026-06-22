// ── Library includes ──────────────────────────────────────────────────────────
#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_log.h>
#include <tensorflow/lite/schema/schema_generated.h>

// ── Generated headers — must be in the same folder as this .ino file ─────────
#include "ORBIT_model.h"       // model_data[] byte array
#include "ORBIT_normalizer.h"  // NUM_FEATURES, FEATURE_MEAN[], FEATURE_STD[]

// ── Class names — must match label order used during training ────────────────
// 0=squat, 1=lunge, 2=jump — same order as CLASS_NAMES in Python
const int   NUM_CLASSES = 3;
const char* CLASS_NAMES[NUM_CLASSES] = {"squat", "lunge", "jump"};

// ── MyoWare / EMG settings ───────────────────────────────────────────────────
const int emgPin = A0;

const int WINDOW_SIZE  = 200;  // 200 samples = 2 seconds at 100 Hz
const int NUM_CHANNELS = 1;    // only one EMG voltage channel
const int DELAY_MS     = 10;   // 10 ms = 100 Hz sample rate

// ── TFLite runtime objects ───────────────────────────────────────────────────
const int TENSOR_ARENA_SIZE = 10 * 1024;
uint8_t tensor_arena[TENSOR_ARENA_SIZE];

tflite::AllOpsResolver     resolver;
const tflite::Model*       tflite_model  = nullptr;
tflite::MicroInterpreter*  interpreter   = nullptr;
TfLiteTensor*              input_tensor  = nullptr;
TfLiteTensor*              output_tensor = nullptr;

// ── EMG data buffer ──────────────────────────────────────────────────────────
// Stores one captured EMG window: 200 voltage samples
float emg_window[WINDOW_SIZE];

// ─────────────────────────────────────────────────────────────────────────────
// Feature extraction — must match Python's extract_features() function.
// Feature order:
// [voltage_mean, voltage_std, voltage_rms, voltage_p2p]
// ─────────────────────────────────────────────────────────────────────────────
void extractFeatures(float* features) {
  float sum    = 0.0f;
  float sq_sum = 0.0f;
  float min_v  =  1e9f;
  float max_v  = -1e9f;

  // First pass: mean, rms numerator, min, max
  for (int i = 0; i < WINDOW_SIZE; i++) {
    float v = emg_window[i];

    sum    += v;
    sq_sum += v * v;

    if (v < min_v) min_v = v;
    if (v > max_v) max_v = v;
  }

  float mean = sum / WINDOW_SIZE;
  float rms  = sqrt(sq_sum / WINDOW_SIZE);

  // Second pass: standard deviation
  float var_sum = 0.0f;

  for (int i = 0; i < WINDOW_SIZE; i++) {
    float diff = emg_window[i] - mean;
    var_sum += diff * diff;
  }

  float std_dev = sqrt(var_sum / WINDOW_SIZE);
  float p2p     = max_v - min_v;

  features[0] = mean;
  features[1] = std_dev;
  features[2] = rms;
  features[3] = p2p;
}

// ─────────────────────────────────────────────────────────────────────────────
// Apply the same normalization as Python's StandardScaler.
// ─────────────────────────────────────────────────────────────────────────────
void normalizeFeatures(float* features) {
  for (int i = 0; i < NUM_FEATURES; i++) {
    features[i] = (features[i] - FEATURE_MEAN[i]) / FEATURE_STD[i];
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// Capture one 2-second MyoWare EMG window.
// ─────────────────────────────────────────────────────────────────────────────
void captureEMGWindow() {
  for (int i = 0; i < WINDOW_SIZE; i++) {
    int adcValue = analogRead(emgPin);
    float voltage = adcValue * (3.3f / 4095.0f);

    emg_window[i] = voltage;

    delay(DELAY_MS);
  }
}

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("=== ORBIT MyoWare EMG Classifier Starting Up ===");

  analogReadResolution(12);  // 0 to 4095 on Nano 33 BLE boards

  // ── Quick A0 check ─────────────────────────────────────────────────────────
  int testValue = analogRead(emgPin);

  if (testValue <= 5) {
    Serial.println("WARNING: A0 is reading near 0. Check MyoWare power, GND, and ENV wire.");
  } 
  else if (testValue >= 4090) {
    Serial.println("WARNING: A0 is reading near max voltage. Make sure signal is not above 3.3V.");
  } 
  else {
    Serial.println("[OK] A0 is reading a signal.");
  }

  // ── Initialize TFLite ─────────────────────────────────────────────────────
  tflite_model = tflite::GetModel(model_data);

  if (tflite_model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("FATAL: TFLite model version mismatch.");
    while (1);
  }

  static tflite::MicroInterpreter static_interpreter(
      tflite_model, resolver, tensor_arena, TENSOR_ARENA_SIZE);

  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("FATAL: TFLite tensor allocation failed.");
    Serial.println("Increase TENSOR_ARENA_SIZE and re-upload.");
    while (1);
  }

  input_tensor  = interpreter->input(0);
  output_tensor = interpreter->output(0);

  Serial.print("[OK] TFLite ready. Tensor arena used: ");
  Serial.print(interpreter->arena_used_bytes());
  Serial.println(" bytes.");

  Serial.println();
  Serial.println("Send any character to capture one EMG window.");
  Serial.println("Then perform the exercise during the 2-second capture.");
  Serial.println();
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  // Wait until user sends a character
  if (!Serial.available()) {
    return;
  }

  while (Serial.available()) {
    Serial.read();  // clear input
  }

  Serial.println("# GET READY...");
  delay(600);

  Serial.println("# CAPTURE NOW — do the exercise!");

  // ── Capture EMG window ────────────────────────────────────────────────────
  captureEMGWindow();

  Serial.println("# Capture complete. Extracting features...");

  // ── Extract features ──────────────────────────────────────────────────────
  float features[NUM_FEATURES];
  extractFeatures(features);

  Serial.print("Raw features: ");
  for (int i = 0; i < NUM_FEATURES; i++) {
    Serial.print(features[i], 4);
    if (i < NUM_FEATURES - 1) Serial.print(", ");
  }
  Serial.println();

  // ── Normalize features ────────────────────────────────────────────────────
  normalizeFeatures(features);

  // ── Copy features into the TFLite input tensor ────────────────────────────
  for (int i = 0; i < NUM_FEATURES; i++) {
    input_tensor->data.f[i] = features[i];
  }

  // ── Run inference ─────────────────────────────────────────────────────────
  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("ERROR: TFLite inference failed.");
    return;
  }

  // ── Read output probabilities and find winner ─────────────────────────────
  float max_confidence  = 0.0f;
  int predicted_class = 0;

  Serial.print("Scores: ");

  for (int i = 0; i < NUM_CLASSES; i++) {
    float conf = output_tensor->data.f[i];

    Serial.print(CLASS_NAMES[i]);
    Serial.print("=");
    Serial.print(conf * 100, 1);
    Serial.print("%  ");

    if (conf > max_confidence) {
      max_confidence = conf;
      predicted_class = i;
    }
  }

  Serial.println();

  Serial.print(">>> EXERCISE: ");
  Serial.print(CLASS_NAMES[predicted_class]);
  Serial.print("  (");
  Serial.print(max_confidence * 100, 1);
  Serial.println("% confident)");
  Serial.println();

  delay(800);
}