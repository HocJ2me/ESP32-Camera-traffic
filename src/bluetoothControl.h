#include <ArduinoJson.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void bluetoothSetup() {
  SerialBT.begin("Camera giao thông"); //Bluetooth device name
  Serial.println("Da khoi dong thanh cong bluetooth!");
}

StaticJsonDocument<100> doc;
String jsonString = "";

long char2L(String str)
{
  long result = 0;
  for (int i = 0; i < str.length(); ++i)
  {
    result = result*10 + str[i] - '0';
  }
  Serial.println(String(result));
  return result;
}

void bluetoothLoop() {
  delay(30);
  if (SerialBT.available()) {
    jsonString = SerialBT.readString();
    Serial.println("Du lieu: " + jsonString);
    startOpenCamera = true;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, jsonString);
  
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
     Serial.println(error.f_str());
      return;
    }
    String timeData = doc["time"];
    rtc.setTime(char2L(timeData)); 
    SerialBT.end();
  }
  
}