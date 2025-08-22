#pragma once
#include <WebServer.h>
#include "esp_camera.h"

class CamWeb {
public:
  void begin();
  void loop();

private:
  WebServer server{80};
  void handleRoot();
  void handleJPG();
};