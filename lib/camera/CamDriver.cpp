#include "CamDriver.h"
#include "camera_pins.h"

static uint32_t chooseXclk() {
#ifdef CAM_XCLK_FREQ
  return CAM_XCLK_FREQ;
#else
  return 12000000;
#endif
}

bool CamDriver::setupConfig() {
  memset(&conf, 0, sizeof(conf));
  fillCameraPins(conf);

  conf.ledc_channel = LEDC_CHANNEL_0;
  conf.ledc_timer   = LEDC_TIMER_0;
  conf.xclk_freq_hz = chooseXclk();

  conf.pixel_format = PIXFORMAT_JPEG;
conf.frame_size   = FRAMESIZE_QVGA;     // QVGA para empezar fluido
conf.jpeg_quality = 12;                 // 10–15 = calidad/media tasa
conf.fb_count     = 2;                  // 2 buffers para pipeline
conf.grab_mode    = CAMERA_GRAB_LATEST; // evita lag de cola

if (psramFound()) {
  conf.fb_location = CAMERA_FB_IN_PSRAM;
}


  return true;
}

bool CamDriver::begin() {
  if (!setupConfig()) return false;

  Serial.printf("[Cam] XCLK=%u Hz @ GPIO%d\n", conf.xclk_freq_hz, conf.pin_xclk);
  Serial.printf("[Cam] SCCB SDA=%d SCL=%d\n", conf.pin_sccb_sda, conf.pin_sccb_scl);

  esp_err_t err = esp_camera_init(&conf);
  if (err != ESP_OK) {
    Serial.printf("[Cam] init FAIL: 0x%x\n", err);
    return false;
  }

  if (auto s = esp_camera_sensor_get()) {
    Serial.printf("[Cam] init OK | PID=0x%02X VER=0x%02X MID=0x%02X%02X\n",
      s->id.PID, s->id.VER, s->id.MIDH, s->id.MIDL);
    // opcional: patrón de barras para validar pipeline (no capturamos, solo enciende el modo)
    s->set_colorbar(s, 1);
  } else {
    Serial.println("[Cam] init OK pero sensor=null (raro)");
  }
  return true;
}

camera_fb_t* CamDriver::captureJpeg() {
  return esp_camera_fb_get();
}
void CamDriver::release(camera_fb_t* fb) {
  if (fb) esp_camera_fb_return(fb);
}
sensor_t* CamDriver::sensor() { return esp_camera_sensor_get(); }
bool CamDriver::setFrameSize(framesize_t fs) {
  if (auto s = esp_camera_sensor_get()) return s->set_framesize(s, fs) == 0;
  return false;
}
void CamDriver::setMirrorFlip(bool h, bool v) {
  if (auto s = esp_camera_sensor_get()) {
    s->set_hmirror(s, h ? 1 : 0);
    s->set_vflip(s,   v ? 1 : 0);
  }
}

