#pragma once
#include "esp_camera.h"

inline void setupCameraPins(camera_config_t &config) {
  config.pin_pwdn  = -1;   //  PWDN/RESET
  config.pin_reset = -1;

  config.pin_xclk      = 15;
  config.pin_sccb_sda  = 4;  // SIOD
  config.pin_sccb_scl  = 5;  // SIOC

  config.pin_d7 = 18;
  config.pin_d6 = 16;
  config.pin_d5 = 14;
  config.pin_d4 = 12;
  config.pin_d3 = 10;
  config.pin_d2 = 8;
  config.pin_d1 = 9;
  config.pin_d0 = 11;

  config.pin_vsync = 6;
  config.pin_href  = 7;
  config.pin_pclk  = 13;
}