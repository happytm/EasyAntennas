// Simple moving average filter (lightweight)
#include <Arduino.h>

// ==== CONFIGURATION ====
const int SAMPLE_SIZE = 32;  // Number of samples for averaging

// ==== FILTER STATE ====
int sampleBuffer[SAMPLE_SIZE] = {0};
int sampleIndex = 0;
long sum = 0;  // Use long to prevent overflow

// ==== FILTER FUNCTION ====
int movingAverageFilter(int sample) {
  // Subtract the oldest sample from sum
  sum -= sampleBuffer[sampleIndex];

  // Add the new sample to sum and buffer
  sampleBuffer[sampleIndex] = sample;
  sum += sample;

  // Move index forward (circular buffer)
  sampleIndex = (sampleIndex + 1) % SAMPLE_SIZE;

  // Return average
  return sum / SAMPLE_SIZE;
}

// ==== SETUP ====
void setup() {
  Serial.begin(115200);
}

// ==== LOOP ====
void loop() {
  // Example: read analog pin A0
  int sensorValue = analogRead(A0);

  // Get smoothed value
  int smoothed = movingAverageFilter(sensorValue);

  Serial.print("Input: "); Serial.print(sensorValue);
  Serial.print(" -> Smoothed: "); Serial.println(smoothed);

  delay(10);  // Adjust loop timing / sample rate as needed
}
