#pragma once
#include <WebServer.h>
#include "CamDriver.h"

class CamWeb {
public:
  explicit CamWeb(CamDriver& cam) : cam_(cam) {}
  void begin();
  void loop();

private:
  CamDriver& cam_;
  WebServer server{80};
  void handleRoot();
  void handleJpg();
  void handleStream();
};
