#pragma once
#include <Arduino.h>
#include <Wire.h>

inline void sccbScan(int sda, int scl) {
  Serial.printf("[SCCB] scan SDA=%d SCL=%d\n", sda, scl);
  Wire.begin(sda, scl, 100000);
  uint8_t found = 0;
  for (uint8_t a = 0x08; a < 0x78; ++a) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  - device @ 0x%02X\n", a);
      found++;
    }
  }
  if (!found) Serial.println("  - none");
}
