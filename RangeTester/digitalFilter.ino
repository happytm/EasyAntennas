// General-purpose digital filter (safe version, no dynamic memory)
#include <Arduino.h>

// ==== CONFIGURATION ====
const int SAMPLE_SIZE = 64;        // Number of samples in buffer
const int AVERAGE_SIZE = 32;       // Number of samples for average
const int VARIANCE_INTEGRATOR = 3; // Number of variance samples to sum

// ==== FILTER BUFFERS ====
int sampleBuffer[SAMPLE_SIZE] = {0};
int averageBuffer[AVERAGE_SIZE] = {0};
int varianceBuffer[SAMPLE_SIZE] = {0};

// ==== FILTER STATE ====
int sampleIndex = 0;
int averageIndex = 0;

int averageValue = 0;
int varianceValue = 0;

// ==== FILTER FUNCTION ====
int digitalFilter(int sample) {
  // Store new sample in circular buffer
  sampleBuffer[sampleIndex] = sample;
  sampleIndex = (sampleIndex + 1) % SAMPLE_SIZE;

  // Compute moving average
  int sum = 0;
  for (int i = 0; i < AVERAGE_SIZE; i++) {
    int idx = (sampleIndex - i - 1 + SAMPLE_SIZE) % SAMPLE_SIZE;
    sum += sampleBuffer[idx];
  }
  averageValue = sum / AVERAGE_SIZE;

  // Store average in average buffer (optional, could be removed if not needed)
  averageBuffer[averageIndex] = averageValue;
  averageIndex = (averageIndex + 1) % AVERAGE_SIZE;

  // Compute variance sample
  int varianceSample = (sample - averageValue) * (sample - averageValue);
  varianceBuffer[sampleIndex] = varianceSample;

  // Compute variance integral (sum of last N variance samples)
  int varSum = 0;
  for (int i = 0; i < VARIANCE_INTEGRATOR; i++) {
    int idx = (sampleIndex - i - 1 + SAMPLE_SIZE) % SAMPLE_SIZE;
    varSum += varianceBuffer[idx];
  }
  varianceValue = varSum;

  return varianceValue; // Return filtered/variance-tracked output
}

// ==== SETUP ====
void setup() {
  Serial.begin(115200);
}

// ==== LOOP ====
void loop() {
  // Example: read analog pin A0
  int sensorValue = analogRead(A0);

  // Get filtered value
  int filtered = digitalFilter(sensorValue);

  Serial.print("Input: "); Serial.print(sensorValue);
  Serial.print(" -> Filtered variance: "); Serial.println(filtered);

  delay(10); // Adjust for your sampling rate
}
