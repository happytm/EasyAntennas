/*
  Adaptive Kalman Filter for RSSI
  Single-file Arduino/ESP32/ESP8266 example
  Dynamically adjusts filter for fast/slow RSSI changes
*/

#include <WiFi.h>  // For ESP32. Use <ESP8266WiFi.h> for ESP8266

// Wi-Fi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Adaptive SimpleKalmanFilter class
class AdaptiveKalmanFilter {
private:
  float Q;    // Process noise (adaptive)
  float R;    // Measurement noise
  float P;    // Estimation error
  float K;    // Kalman gain
  float X;    // Current estimate
  float lastMeasurement;

public:
  AdaptiveKalmanFilter(float processNoise, float measurementNoise, float estimatedError, float initialValue) {
    Q = processNoise;
    R = measurementNoise;
    P = estimatedError;
    X = initialValue;
    lastMeasurement = initialValue;
  }

  float update(float measurement) {
    // Dynamically adjust Q based on change magnitude
    float diff = abs(measurement - lastMeasurement);
    Q = 0.01 + diff * 0.05;  // Small change = smoother, big change = faster adaptation

    lastMeasurement = measurement;

    // Prediction update
    P = P + Q;

    // Measurement update
    K = P / (P + R);
    X = X + K * (measurement - X);
    P = (1 - K) * P;

    return X;
  }

  float getEstimate() const {
    return X;
  }
};

// Initialize adaptive Kalman filter for RSSI
AdaptiveKalmanFilter rssiFilter(0.01, 2.0, 1.0, -60.0); // Tune initial Q, R

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Connecting to Wi-Fi...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
}

void loop() {
  int rssi = WiFi.RSSI();  // Get raw RSSI
  float filteredRssi = rssiFilter.update(rssi);

  Serial.print("Raw RSSI: ");
  Serial.print(rssi);
  Serial.print("  -> Filtered RSSI: ");
  Serial.println(filteredRssi);

  delay(500); // half-second update
}
