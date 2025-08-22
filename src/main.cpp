#include <Arduino.h>
#include <WiFi.h>
#include "esp_camera.h"

#include "wifi_secrets.h"
#include "camera_pins.h"
#include "CamWeb.h"

CamWeb web;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Conectando a %s", WIFI_SSID);
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - t0 > 20000) break; // 20 s timeout
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWiFi OK: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nNo se pudo conectar a WiFi (continuo sin red).");
  }
}

bool initCamera() {
  camera_config_t config = {};
  setupCameraPins(config);

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
#ifdef CAM_XCLK_FREQ
  config.xclk_freq_hz = CAM_XCLK_FREQ;
#else
  config.xclk_freq_hz = 12000000; // prueba 12 MHz; si falla, 20 MHz
#endif
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_QVGA; // arranca pequeño para validar
  config.jpeg_quality = 12;
  config.fb_count     = 2;
  config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;

  if (psramFound()) {
    config.frame_size  = FRAMESIZE_VGA; // con PSRAM puedes subir
    config.fb_location = CAMERA_FB_IN_PSRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }

  // Ajustes básicos del sensor (opcional)
  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    s->set_brightness(s, 0);   // -2..2
    s->set_contrast(s, 0);     // -2..2
    s->set_saturation(s, 0);   // -2..2
    s->set_hmirror(s, 1);      // voltea horizontal si tu imagen sale invertida
    s->set_vflip(s, 0);
  }

  return true;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nBooting...");

  if (!initCamera()) {
    Serial.println(">> Revisa pinout/FPC/XCLK. Bajando a QQVGA...");
    // Prueba rápida en muy baja resolución:
    // (reintento simple)
    // Podríamos reintentar cambiando XCLK a 20 MHz aquí si hace falta.
  }

  connectWiFi();
  web.begin();
  Serial.println("Servidor listo. Abre http://<IP>/");
}

void loop() {
  web.loop();
  delay(5);
}
