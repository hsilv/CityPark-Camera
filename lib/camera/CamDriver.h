#pragma once
#include <Arduino.h>
#include "esp_camera.h"

class CamDriver {
public:
  bool begin();                       // ya lo tienes
  camera_fb_t* captureJpeg();         // ↓ añade
  void release(camera_fb_t* fb);
  sensor_t* sensor();
  bool setFrameSize(framesize_t fs);
  void setMirrorFlip(bool h, bool v);

private:
  camera_config_t conf{};
  bool setupConfig();
};
