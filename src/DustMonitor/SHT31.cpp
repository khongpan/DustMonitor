#include "Arduino.h"
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include "BlynkSimpleEsp32.h"
#define BLYNK_PRINT Serial
#define VCC 19
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
float t, h;
void TaskSHT31(void *pvParameters)
{
  (void) pvParameters;

  Adafruit_SHT31 sht31 = Adafruit_SHT31();
  for (;;)
  {
    if (! sht31.begin(0x44))
    {
      Serial.println("Couldn't find SHT31");
      while (1) vTaskDelay(1);
    }
    t = sht31.readTemperature();
    h = sht31.readHumidity();

    if (! isnan(t))
    {
      Serial.print("SHT31 Temp *C = "); Serial.println(t);
    }
    else
    {
      Serial.println("Failed to read temperature SHT31");
    }

    if (! isnan(h))
    {
      Serial.print("SHT31 Hum. % = "); Serial.println(h);
    }
    else
    {
      Serial.println("Failed to read humidity SHT31 ");
    }
    Blynk.virtualWrite(V1,t);
    Blynk.virtualWrite(V2,h);
    vTaskDelay(15000);
  }
}
void SHT31Read(float *Temp_value, float *Humid_value) {
  *Temp_value = t;
  *Humid_value = h;
}
void SHT31Setup() {

  //Adafruit_SHT31 sht31 = Adafruit_SHT31();
  pinMode (VCC, OUTPUT);
  digitalWrite(VCC, HIGH);
  xTaskCreatePinnedToCore(
    TaskSHT31
    ,  "TaskSHT31"
    ,  4000
    ,  NULL
    ,  2
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);
}
