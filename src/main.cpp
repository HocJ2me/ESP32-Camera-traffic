/*
 * Đầu tiên tạo bluetooth client để chờ điện thoại kết nối
 * Chương trình sẽ chờ cho đến khi có dữ liệu gửi từ điện thoại
 * Bắt đầu chụp ảnh
 */
#include <Arduino.h>

 #include "config.h"
 #include "timeFunction.h"
 #include "takePhotoESP.h"
 #include "bluetoothControl.h"
 
void setup() {
  Serial.begin(115200);
  bluetoothSetup();
  cameraSetup();
}

void loop() {
  bluetoothLoop();
  cameraLoop();
}