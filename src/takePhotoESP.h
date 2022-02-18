#include "FS.h"       // SD Card
#include "SD_MMC.h"   // SD Card

#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory

// define the number of bytes you want to access
#define EEPROM_SIZE 4

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22



#define FLASH_GPIO_NUM 4

#define FLASH_BRIGHTNESS 0

const int freq = 5000;
const int ledChannel = LEDC_CHANNEL_6;




void cameraSetup() {
  //At first init SD_MMC because otherwise begin() returns false
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD_MMC card attached");
    return;
  }

  Serial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

  delay(250);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG ;//PIXFORMAT_GRAYSCALE

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;//0-63, lower means higher quality
    config.fb_count = 1;
  }

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

}

String timeNow()
{
  return String(rtc.getTime("%F %H-%M-%S"));
}

void writeFile(fs::FS &fs) {
  String dateTimeString = "";
  dateTimeString = timeNow()  ;
  String path = "/" + dateTimeString + ".jpg";
  Serial.printf("Writing file: %s\n", path.c_str());

  File file = fs.open(path.c_str(), FILE_WRITE);

  
  
  Serial.println("take photo..");
  // Take Picture with Camera
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  vTaskDelay(50);
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  file.write(fb->buf, fb->len); // payload (image), payload length

  esp_camera_fb_return(fb);
}



int i = 0;
unsigned long previousMillis = 0;
unsigned long previousTakePhoto = 0;
void cameraLoop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= timePauseTakePhoto) 
  {
    if(currentMillis - previousMillis <= timeTakePhoto + timePauseTakePhoto)
    {
        unsigned long currentTakePhoto = millis();
        if (currentTakePhoto - previousTakePhoto >= timeDuringTake) 
        {
              if (startOpenCamera) writeFile(SD_MMC);
              previousTakePhoto = currentTakePhoto;
        }
    }
    else
    {
      previousMillis = currentMillis;
    }
  }
}