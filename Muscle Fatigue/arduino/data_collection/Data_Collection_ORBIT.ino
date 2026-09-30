const int emgPin = A0;

const int WINDOW_SIZE = 200;    // # of samples = seconds * 100 Hz(sampling rate)
const int DELAY_MS    = 10;    // 10 ms between samples = 100 Hz

void setup() {
  Serial.begin(115200);
  while (!Serial);

  analogReadResolution(12); // 0 to 4095 for Nano 33 BLE boards

  int testValue = analogRead(emgPin);

  if (testValue <= 5) {
    Serial.println("WARNING: A0 is reading near 0. Check GND, signal wire, or sensor power.");
  } 
  else if (testValue >= 4090) {
    Serial.println("WARNING: A0 is reading near max voltage. Check wiring and make sure signal is not above 3.3V.");
  } 
  else {
    Serial.println("A0 is reading a signal.");
  }

  Serial.println("=== Gesture Data Collector ===");
  Serial.println("Instructions:");
  Serial.println("  1. Type any character and press Send");
  Serial.println("  2. Wait for 'CAPTURE NOW'");
  Serial.println("  3. Perform your gesture within 0.5 seconds");
  Serial.println("  4. Copy the output row into your label file");
  Serial.println("  5. Repeat 60 times per gesture class");
  Serial.println();
  Serial.println("Ready. Send any character to start.");
}

void loop() {
  if (Serial.available()) {
    while (Serial.available()) Serial.read();  // flush the input

    // Brief pause so you can get your hand ready
    Serial.println("# GET READY...");
    delay(600);
    Serial.println("# CAPTURE NOW — do your gesture!");

    // Collect WINDOW_SIZE samples at ~100 Hz
    float samples[WINDOW_SIZE];
    for (int i = 0; i < WINDOW_SIZE; i++) {
      // Wait for fresh data from both sensors
      int adcValue = analogRead(emgPin);
      float voltage = adcValue * (3.3/4095.0);

      samples[i] = voltage;

      delay(DELAY_MS);
    }

    // Print all 200 samples as one long CSV row
    for (int i = 0; i < WINDOW_SIZE; i++) {
      Serial.print(samples[i], 4);
      if (i < WINDOW_SIZE - 1) Serial.print(",");
    }
    Serial.println();  // end of this data row

    Serial.println("# DONE. Send any character for next capture.");
    Serial.println();
  }
}
