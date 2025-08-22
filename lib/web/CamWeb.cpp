#include "CamWeb.h"
#include <WiFi.h>

static const char BOUNDARY[] = "frame";

void CamWeb::handleRoot() {
  static const char html[] PROGMEM =
    "<!doctype html><html><head><meta name='viewport' content='width=device-width'>"
    "<title>ESP32-S3 Cam</title><style>body{font-family:sans-serif;margin:20px}img{max-width:100%}</style>"
    "</head><body>"
    "<h2>ESP32-S3 Camera</h2>"
    "<p><button onclick=\"document.getElementById('snap').src='/jpg?ts='+Date.now()\">Snapshot</button></p>"
    "<p><img id='snap' src='/jpg' alt='snapshot'></p>"
    "<hr><h3>Stream</h3>"
    "<p><img id='stream' src='/stream' alt='stream'></p>"
    "</body></html>";
  server.send_P(200, "text/html", html);
}

void CamWeb::handleJpg() {
  camera_fb_t* fb = cam_.captureJpeg();
  if (!fb) { server.send(500, "text/plain", "Camera capture failed"); return; }
  server.sendHeader("Cache-Control", "no-store");
  server.setContentLength(fb->len);
  server.send(200, "image/jpeg", "");
  server.sendContent((const char*)fb->buf, fb->len);
  cam_.release(fb);
}

void CamWeb::handleStream() {
  WiFiClient client = server.client();
  String hdr = "HTTP/1.1 200 OK\r\n"
               "Content-Type: multipart/x-mixed-replace; boundary=" + String(BOUNDARY) + "\r\n"
               "Cache-Control: no-store\r\n"
               "Pragma: no-cache\r\n"
               "Connection: close\r\n\r\n";
  client.write((const uint8_t*)hdr.c_str(), hdr.length());

  while (client.connected()) {
    camera_fb_t* fb = cam_.captureJpeg();
    if (!fb) break;

    String head = "--" + String(BOUNDARY) + "\r\n"
                  "Content-Type: image/jpeg\r\n"
                  "Content-Length: " + String(fb->len) + "\r\n\r\n";
    client.write((const uint8_t*)head.c_str(), head.length());
    client.write(fb->buf, fb->len);
    client.write((const uint8_t*)"\r\n", 2);
    cam_.release(fb);

    delay(30); // ~33 fps teórico, baja/sube para estabilizar
  }
}

void CamWeb::begin() {
  server.on("/",       [this]{ handleRoot();   });
  server.on("/jpg",    [this]{ handleJpg();    });
  server.on("/stream", [this]{ handleStream(); });
  server.begin();
}

void CamWeb::loop() { server.handleClient(); }
