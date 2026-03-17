// Hybrid Moving Average + Variance Filter
#include <Arduino.h>

// ==== CONFIGURATION ====
const int SAMPLE_SIZE = 32;       // Number of samples for moving average
const int VARIANCE_WINDOW = 4;    // Number of variance samples to sum (optional sensitivity)

// ==== FILTER STATE ====
int sampleBuffer[SAMPLE_SIZE] = {0};
int sampleIndex = 0;

long sum = 0;                    // Sum of samples for average
int varianceBuffer[VARIANCE_WINDOW] = {0};
int varIndex = 0;

// ==== FILTER FUNCTION ====
// Returns variance (or 0 if not enough samples yet)
int hybridFilter(int sample, int &smoothed) {
  // --- Update moving average ---
  sum -= sampleBuffer[sampleIndex];   // Remove oldest sample from sum
  sampleBuffer[sampleIndex] = sample; // Store new sample
  sum += sample;                       // Add new sample to sum
  smoothed = sum / SAMPLE_SIZE;        // Compute average

  // --- Update circular index ---
  sampleIndex = (sampleIndex + 1) % SAMPLE_SIZE;

  // --- Compute variance sample ---
  int varSample = (sample - smoothed) * (sample - smoothed);
  varianceBuffer[varIndex] = varSample;

  // --- Compute variance integral over last VARIANCE_WINDOW samples ---
  int varSum = 0;
  for (int i = 0; i < VARIANCE_WINDOW; i++) {
    varSum += varianceBuffer[i];
  }

  // --- Update variance buffer index ---
  varIndex = (varIndex + 1) % VARIANCE_WINDOW;

  return varSum; // Return variance sum
}

// ==== SETUP ====
void setup() {
  Serial.begin(115200);
}

// ==== LOOP ====
void loop() {
  // Example input: analog pin A0
  int sensorValue = analogRead(A0);
  int smoothedValue = 0;

  // Get smoothed value + variance
  int variance = hybridFilter(sensorValue, smoothedValue);

  Serial.print("Input: "); Serial.print(sensorValue);
  Serial.print(" -> Smoothed: "); Serial.print(smoothedValue);
  Serial.print(" -> Variance: "); Serial.println(variance);

  delay(10); // Adjust for sampling rate
}
