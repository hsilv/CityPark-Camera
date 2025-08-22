#include "CamWeb.h"

void CamWeb::handleRoot() {
  static const char html[] PROGMEM =
    "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'/>"
    "<title>ESP32-S3 Cam</title>"
    "<style>body{font-family:sans-serif;margin:20px}img{max-width:100%}</style>"
    "</head><body>"
    "<h2>ESP32-S3 Camera</h2>"
    "<button onclick=\"document.getElementById('img').src='/jpg?ts='+Date.now()\">Snapshot</button>"
    "<p><img id='img' src='/jpg' alt='snapshot'></p>"
    "</body></html>";
  server.send_P(200, "text/html", html);
}

void CamWeb::handleJPG() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void CamWeb::begin() {
  server.on("/",  [this](){ handleRoot(); });
  server.on("/jpg", [this](){ handleJPG(); });
  server.begin();
}

void CamWeb::loop() {
  server.handleClient();
}