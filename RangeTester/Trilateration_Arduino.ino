#include <Arduino.h>
#include <math.h>

struct Point {
  double x;
  double y;
  double z;
};

// Trilateration function
bool trilaterate(const Point &p1, double r1,
                 const Point &p2, double r2,
                 const Point &p3, double r3,
                 Point &result) {

  double exx = p2.x - p1.x;
  double exy = p2.y - p1.y;
  double exz = p2.z - p1.z;
  double d = sqrt(exx*exx + exy*exy + exz*exz);
  if (d == 0) return false;

  double ex[3] = {exx/d, exy/d, exz/d};

  double p3p1x = p3.x - p1.x;
  double p3p1y = p3.y - p1.y;
  double p3p1z = p3.z - p1.z;

  double i = ex[0]*p3p1x + ex[1]*p3p1y + ex[2]*p3p1z;

  double auxx = p3p1x - i*ex[0];
  double auxy = p3p1y - i*ex[1];
  double auxz = p3p1z - i*ex[2];
  double auxNorm = sqrt(auxx*auxx + auxy*auxy + auxz*auxz);
  if (auxNorm == 0) return false;

  double ey[3] = {auxx/auxNorm, auxy/auxNorm, auxz/auxNorm};
  double ez[3] = {ex[1]*ey[2] - ex[2]*ey[1],
                  ex[2]*ey[0] - ex[0]*ey[2],
                  ex[0]*ey[1] - ex[1]*ey[0]};
  double j = ey[0]*p3p1x + ey[1]*p3p1y + ey[2]*p3p1z;

  double x = (r1*r1 - r2*r2 + d*d) / (2*d);
  double y = (r1*r1 - r3*r3 + i*i + j*j)/(2*j) - (i/j)*x;
  double z2 = r1*r1 - x*x - y*y;
  if (z2 < 0) return false;
  double z = sqrt(z2);

  result.x = p1.x + x*ex[0] + y*ey[0] + z*ez[0];
  result.y = p1.y + x*ex[1] + y*ey[1] + z*ez[1];
  result.z = p1.z + x*ex[2] + y*ey[2] + z*ez[2];

  return true;
}

// --- Anchor positions ---
Point anchor1 = {0, 0, 0};
Point anchor2 = {5, 0, 0};
Point anchor3 = {0, 5, 0};

// Example distance reading function (replace with your sensor code)
double readDistance(Point anchor) {
  // Placeholder: return simulated distance
  // Replace with real UWB/BLE distance measurement
  return sqrt(pow(anchor.x - 2.5, 2) + pow(anchor.y - 2.5, 2) + pow(anchor.z - 0, 2));
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Read distances continuously
  double r1 = readDistance(anchor1);
  double r2 = readDistance(anchor2);
  double r3 = readDistance(anchor3);

  Point currentPosition;
  if (trilaterate(anchor1, r1, anchor2, r2, anchor3, r3, currentPosition)) {
    Serial.print("X: "); Serial.print(currentPosition.x, 2);
    Serial.print(" Y: "); Serial.print(currentPosition.y, 2);
    Serial.print(" Z: "); Serial.println(currentPosition.z, 2);
  } else {
    Serial.println("No valid position found.");
  }

  delay(200); // Adjust update rate (5 Hz here)
}
