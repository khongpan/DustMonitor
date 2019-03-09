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

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskDustSensor(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
    
  If you want to know what pin the on-board LED is connected to on your ESP32 model, check
  the Technical Specs of your board.
*/

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    hpma115S0.StartParticleMeasurement();
    vTaskDelay(15000);
    if (hpma115S0.ReadParticleMeasurement(&pm2_5, &pm10)) {
        Serial.print("PM 2.5:\t" + String(pm2_5) + " ug/m3\t" );
        Serial.println("\tPM 10:\t" + String(pm10) + " ug/m3" );
    hpma115S0.StopParticleMeasurement();
    vTaskDelay(45000);  
  }
  }
}
void DustSensorRead(int *PM2_5, int *PM10){
  *PM2_5 = pm2_5;
  *PM10 = pm10;
}

void DustSensorSetup() {
  hpmaSerial.begin(9600,SERIAL_8N1,12,13);
  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskDustSensor
    ,  "TaskDustSensor"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}
