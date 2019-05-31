#include "Arduino.h"
#include <Wire.h>
#include "CCS811.h"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include "Network.h"
#include"DustSensor.h"
#include"SHT31.h"
#include "HDC1080.h"
#include "BlynkApi.h"
#include "BlynkSimpleEsp32.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

//char auth[] = "a184d6856ff9481baa671e41c3bae2c2";
//char auth[] = "8895bb7fae3248a78296aada682b020a";
char auth[] = "8aff28e60ffb45b6b5c7b084403bcd5a";//login sunti.58
//char auth[] = "654b6ab5d9fa403b9baad2eebf9591fb"; //login facebook
//char auth[] = "a78f85ec5e6d4300b9ad74fa434362de"; local server
//char ssid[] = "Ball";
//char pass[] = "12345678";
int rowIndex = 0;
const int buttonpin = 0;
//int buttonstate = 0;
float temp_blynk, humid_blynk, temp_value_blynk, humid_value_blynk;
int hpma_value_pm10_blynk, hpma_value_pm2_5_blynk, hpma_value_PM2_5_blynk, hpma_value_PM10_blynk;
uint16_t eco2_blynk, etvoc_blynk, errstat_blynk, raw_blynk;
int cnt = 0;


WiFiClient _blynkWifiClient;
BlynkArduinoClient _blynkTransport(_blynkWifiClient);
BlynkWifi Blynk(_blynkTransport);
BlynkApi blynk();
BlynkTimer timer;

//BLYNK_WRITE(V1) {
//  int value_v1 = param.asInt();
//  Serial.println(value_v1);
//}
//BLYNK_WRITE(V2) {
//  int value_v2 = param.asInt();
//  Serial.println(value_v2);
//}
//BLYNK_WRITE(V3) {
//  int value_v3 = param.asInt();
//  Serial.println(value_v3);
//}
//BLYNK_WRITE(V4) {
//  int value_v4 = param.asInt();
//  Serial.println(value_v4);
//}
//BLYNK_WRITE(V5) {
//  int value_v5 = param.asInt();
//  Serial.println(value_v5);
//}
//BLYNK_WRITE(V9) {
//  Blynk.virtualWrite(V4, temp_1);
//  int Value_v9 = param.asInt();
//  Serial.println (Value_v9);
//}
//BLYNK_WRITE(V10) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v10 = param.asInt();
//  Serial.println (Value_v10);
//}
//BLYNK_WRITE(V11) {
//  int value_v1 = param.asInt();
//  Serial.println(value_v1);
//}
//BLYNK_WRITE(V12) {
//  int value_v2 = param.asInt();
//  Serial.println(value_v2);
//}
//BLYNK_WRITE(V13) {
//  int value_v3 = param.asInt();
//  Serial.println(value_v3);
//}
//BLYNK_WRITE(V14) {
//  int value_v4 = param.asInt();
//  Serial.println(value_v4);
//}
//BLYNK_WRITE(V15) {
//  int value_v5 = param.asInt();
//  Serial.println(value_v5);
//}
//BLYNK_WRITE(V16) {
//  Blynk.virtualWrite(V4, temp_1);
//  int Value_v9 = param.asInt();
//  Serial.println (Value_v9);
//}
//BLYNK_WRITE(V17) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v10 = param.asInt();
//  Serial.println (Value_v10);
//}
//BLYNK_WRITE(V18) {
//  int value_v5 = param.asInt();
//  Serial.println(value_v5);
//}
//BLYNK_WRITE(V19) {
//  Blynk.virtualWrite(V4, temp_1);
//  int Value_v9 = param.asInt();
//  Serial.println (Value_v9);
//}
//BLYNK_WRITE(V20) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v10 = param.asInt();
//  Serial.println (Value_v10);
//}
//BLYNK_WRITE(V21) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v10 = param.asInt();
//  Serial.println (Value_v10);
//}
//BLYNK_WRITE(V22) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v10 = param.asInt();
//  Serial.println (Value_v10);
//}
//BLYNK_WRITE(V23) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v10 = param.asInt();
//  Serial.println (Value_v10);
//}
//BLYNK_WRITE(V24) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v10 = param.asInt();
//  Serial.println (Value_v10);
//}
//BLYNK_WRITE(V25) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v10 = param.asInt();
//  Serial.println (Value_v10);
//}
//BLYNK_WRITE(V26) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v10 = param.asInt();
//  Serial.println (Value_v10);
//}
//BLYNK_WRITE(V27) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v27 = param.asInt();
//  Serial.println (Value_v27);
//}
//BLYNK_WRITE(V28) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v28 = param.asInt();
//  Serial.println (Value_v28);
//}
//BLYNK_WRITE(V29) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v29 = param.asInt();
//  Serial.println (Value_v29);
//}
//BLYNK_WRITE(V30) {
//  Blynk.virtualWrite(V5, humid_1);
//  int Value_v30 = param.asInt();
//  Serial.println (Value_v30);
//}
void TaskBlynk(void *pvParameters)
{
  (void) pvParameters;

  for (;;)
  {

    //    ccs811.read(&eco2_blynk, &etvoc_blynk, &errstat_blynk, &raw_blynk);
    //    SHT31Read(&temp_value_blynk, &humid_value_blynk);
    //    DustSensorRead(&hpma_value_PM2_5_blynk, &hpma_value_PM10_blynk);


    //    float temp_blynk = random(25,75);
    //    float humid_blynk = random(40,99);
    //    eco2_blynk = random(400,5000);
    //    etvoc_blynk = random(0,2000);
    //
    //    SHT31Read(&temp_value_blynk, &humid_value_blynk);
    //    DustSensorRead(&hpma_value_PM2_5_blynk, &hpma_value_PM10_blynk);



    // Blynk.virtualWrite(V4, rowIndex);
    //    int Value_v9 = param.asInt();
    //    Serial.println (Value_v9);

    //Blynk.virtualWrite(V5, rowIndex);
    //    int Value_v10 = param.asInt();
    //    Serial.println (Value_v10);

    //ccs811.read(&eco2_1, &etvoc_1, &errstat_1, &raw_1);
    //    int buttonstate = digitalRead(buttonpin);
    //    if (buttonstate == LOW) {
    //
    //      cnt++;
    //      if (cnt == 3) {
    //        rowIndex++;
    //      }
    //      Blynk.virtualWrite(V1, rowIndex);
    //      Blynk.virtualWrite(V2, rowIndex);
    //      Blynk.virtualWrite(V3, rowIndex);
    //      Blynk.virtualWrite(V4, rowIndex);
    //      Blynk.virtualWrite(V5, rowIndex);

    //      Blynk.syncVirtual(V2, V3);
    //    } else cnt = 0;
    //
    //    Blynk.syncVirtual(V1);
    Blynk.run();
    timer.run();
  }

}
//void Blynk_loop() {
//  Blynk.virtualWrite(V1, temp_blynk);
//  Blynk.virtualWrite(V2, humid_blynk);
// Blynk.virtualWrite(V3, eco2_blynk);
// Blynk.virtualWrite(V4, etvoc_blynk);
// Blynk.virtualWrite(V5, hpma_value_PM2_5_blynk);
// Blynk.virtualWrite(V6, hpma_value_PM10_blynk);
//}
//void Blynk_loop() {
//  Blynk.virtualWrite(V1,1);
//  Blynk.virtualWrite(V2, 2);
//  Blynk.virtualWrite(V3, 3);
//  Blynk.virtualWrite(V4, 4);
//  Blynk.virtualWrite(V5, 5);
//}
void BlynkSetup() {


  const IPAddress server(192, 168, 100, 202);
  const int httpsPort = 8080;
  //timer.setInterval(10000, Blynk_loop);

  Blynk.config(auth, server, httpsPort);

  //Blynk.begin(auth, ssid, pass);
  //Blynk.config(auth);
  //Blynk.connect();
  while (Blynk.connected() == false) {
    Serial.print(".");
    Blynk.connect();
    //vTaskDelay(1000);
  }


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
