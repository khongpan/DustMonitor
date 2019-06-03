#include "Arduino.h"
#include "hpma115S0.h"
HardwareSerial hpmaSerial(1);
HPMA115S0 hpma115S0(hpmaSerial);

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
unsigned int pm2_5, pm10;


/*---------------------- Tasks ---------------------*/


void TaskDustSensor(void *pvParameters)
{
  (void) pvParameters;




  for (;;)
  {
    vTaskDelay(15000);
    hpma115S0.StartParticleMeasurement();
    vTaskDelay(15000);
    if (hpma115S0.ReadParticleMeasurement(&pm2_5, &pm10)) {
      Serial.print("PM 2.5:\t" + String(pm2_5) + " ug/m3\t" );
      Serial.println("\tPM 10:\t" + String(pm10) + " ug/m3" );
      hpma115S0.StopParticleMeasurement();
      vTaskDelay(30000);
    } else Serial.println("hpma read fail");


  }
}

void DustSensorRead(int *PM2_5_value, int *PM10_value) {
  *PM2_5_value = pm2_5;
  *PM10_value = pm10;
}

void DustSensorSetup() {

  hpmaSerial.begin(9600, SERIAL_8N1, 17, 5);

  xTaskCreatePinnedToCore(
    TaskDustSensor
    ,  "TaskDustSensor"
    ,  4096
    ,  NULL
    ,  2
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);


}
