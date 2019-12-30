#include <ESP32Lib.h>
#include "camera.h"
#include "tools.h"

unsigned char image[40*30][3];
SerialLED gfx;

int imageRes[] = {40, 30};
int currentImage = 1;
int calibration = 100;
volatile bool frameAvailable = false;
#include "effects.h"
#include "server.h"

void webTask(void *p)
{
  while(1)
  {
    handleServer();
    delay(10);   
  }
}

void setup()
{
  Serial.begin(115200);
  cameraInit();
  setCameraParams();
  initServer();
  calculateSinTab();
  calcRainbow();
  gfx.setGamma(2.8f, 2.8f, 2.8f);
  gfx.init(2, 1200, -1, -1);
  initEffects();
  TaskHandle_t xHandle = NULL;
  xTaskCreatePinnedToCore(webTask, "web", 5000, 0,  ( 2 | portPRIVILEGE_BIT ), &xHandle, 0);
}

void loop()
{
  camera_fb_t * fb = esp_camera_fb_get();
  if (fb) 
  {
    downSample(fb->buf);  //fb->width, fb->height, fb->format, fb->buf, fb->len
    frameAvailable = true;
    esp_camera_fb_return(fb);
  };   
  if(frameAvailable) 
  {
    processImage();
    frameAvailable = false;
  }
}
