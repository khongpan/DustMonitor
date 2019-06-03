#include "Arduino.h"
#include <Wire.h>
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
char auth[] = "8895bb7fae3248a78296aada682b020a";
int rowIndex = 0;
const int buttonpin = 0;
int cnt = 0;
BLYNK_WRITE(V5) {
  int pinValue = param.asInt();
  Serial.println(pinValue);
  Serial.println ("k10");
}
void TaskBlynk(void *pvParameters)
{
  (void) pvParameters;

  for (;;)
  {
    Blynk.run();
    //timer.run();
  }
}
void Blynk_loop() {
  Blynk.virtualWrite(V4, rowIndex);
  Blynk.virtualWrite(V5, rowIndex);
  rowIndex++;
}
void BlynkSetup() {
  Blynk.config(auth);
  Blynk.connect();
//  timer.setInterval(10000, Blynk_loop);
  while (Blynk.connect() == false) {
    Serial.print(".");
    vTaskDelay(1000);
  }
  Serial.println("Connected to Blynk server");
  pinMode(buttonpin, INPUT);

  xTaskCreatePinnedToCore(
    TaskBlynk
    ,  "TaskBlynk"
    ,  4096
    ,  NULL
    ,  2
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);
}
