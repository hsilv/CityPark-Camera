#pragma once
#include "esp_camera.h"

#ifndef CAM_XCLK_GPIO
  #define CAM_XCLK_GPIO 10   // prueba 10; si no, cambia a 15 en platformio.ini
#endif

inline void fillCameraPins(camera_config_t& c) {
  c.pin_pwdn  = -1;
  c.pin_reset = -1;

  c.pin_xclk     = CAM_XCLK_GPIO;
  c.pin_sccb_sda = 4;   // SIOD
  c.pin_sccb_scl = 5;   // SIOC

  c.pin_d7 = 18;
  c.pin_d6 = 16;
  c.pin_d5 = 14;
  c.pin_d4 = 12;
  c.pin_d3 = 10;
  c.pin_d2 = 8;
  c.pin_d1 = 9;
  c.pin_d0 = 11;

  c.pin_vsync = 6;
  c.pin_href  = 7;
  c.pin_pclk  = 13;
}
