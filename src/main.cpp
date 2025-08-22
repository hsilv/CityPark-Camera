#include <Arduino.h>
#include "esp_camera.h"
#include "camera_pins.h"

// 👇 esto debe ir al inicio, no dentro de dumpMem()
extern "C" {
  #include "esp_heap_caps.h"
}

static void dumpMem() {
  Serial.printf("PSRAM? %s, size=%u, free_psram=%u, free_int=%u\n",
    psramFound() ? "YES":"NO",
    ESP.getPsramSize(),
    heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
    heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== CAM DIAG CAPTURE ===");
  dumpMem();

  camera_config_t cfg{};
  fillCameraPins(cfg);                // <-- aquí se rellenan todos los GPIO (de tu header)
  cfg.ledc_channel  = LEDC_CHANNEL_0;
  cfg.ledc_timer    = LEDC_TIMER_0;

#ifdef CAM_XCLK_FREQ
  cfg.xclk_freq_hz  = CAM_XCLK_FREQ; // lo defines en platformio.ini
#else
  cfg.xclk_freq_hz  = 20000000;
#endif

  cfg.pixel_format  = PIXFORMAT_JPEG; // para capturar JPG
    cfg.frame_size = FRAMESIZE_QQVGA; // 160x120

  cfg.jpeg_quality  = 12;             // 10–15 ok
  cfg.fb_count      = 2;              // mejor con PSRAM
  cfg.grab_mode     = CAMERA_GRAB_LATEST;
  if (psramFound()) cfg.fb_location = CAMERA_FB_IN_PSRAM;

  Serial.printf("[CFG] XCLK=%u Hz @ GPIO%d | SDA=%d SCL=%d\n",
                cfg.xclk_freq_hz, cfg.pin_xclk, cfg.pin_sccb_sda, cfg.pin_sccb_scl);

  esp_err_t err = esp_camera_init(&cfg);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
  if (s) {
    Serial.printf("Sensor: PID=0x%02X VER=0x%02X MID=%02X%02X\n",
                  s->id.PID, s->id.VER, s->id.MIDH, s->id.MIDL);
    // Si aún no ves nada luego, prueba habilitar barras de color para verificar el pipeline:
     s->set_colorbar(s, 1);
  }

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Capture failed (fb == null)");
    return;
  }

  Serial.printf("Captured %u bytes, w=%u h=%u, format=%u\n",
                (unsigned)fb->len, fb->width, fb->height, fb->format);
  esp_camera_fb_return(fb);

  Serial.println("Done.");
}

void loop() {
  delay(1000);
}
