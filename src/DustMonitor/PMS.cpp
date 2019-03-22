#include "Arduino.h"
#include <PMS.h>
#define  p   18
String pmstr1, pmstr2_5, pmstr10;
int AA,BB;
HardwareSerial PMSSerial(2);
PMS pms(PMSSerial);
PMS::DATA data;
struct pm
{
  //uint16_t pm1_0;
  uint16_t pm2_5;
  uint16_t pm10_0;
};



#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskPMS(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  /*
    Blink
    Turns on an LED on for one second, then off for one second, repeatedly.

    If you want to know what pin the on-board LED is connected to on your ESP32 model, check
    the Technical Specs of your board.
  */



  for (;;) // A Task shall never return or exit.
  {
    struct pm getPM(); {

      struct pm pm_instance;
      int read = 0;

      
      digitalWrite (p, HIGH);
      vTaskDelay(30000); 
      //Serial.print("gg");
      while (!read) {
        if (pms.read(data)) {

          //pm_instance.pm1_0 = data.PM_AE_UG_1_0;
          pm_instance.pm2_5 = data.PM_AE_UG_2_5;
          pm_instance.pm10_0 = data.PM_AE_UG_10_0;
          read = 1;
          // vTaskDelay(1000);
           //Serial.print(".");
        }
      }

      //pmstr1 = String(pm_instance.pm1_0);
      pmstr2_5 = String (pm_instance.pm2_5);
      pmstr10 = String (pm_instance.pm10_0);
 
      //Serial.println("PM 1 : " + pmstr1 );
      Serial.print("PM 2_5 : " + pmstr2_5                    );
      Serial.println("                   PM 10 : " + pmstr10 );
       AA = pm_instance.pm2_5;
       BB = pm_instance.pm10_0 ;
     
     digitalWrite (p, LOW);
    

    }
      vTaskDelay(30000);  // one tick delay (15ms) in between reads for stability
  }
}

void PMSValue(int *Pm2_5, int *Pm10){
  *Pm2_5 = AA ;
  *Pm10 = BB;
}
// the setup function runs once when you press reset or power the board
void PMSSetup() {
  PMSSerial.begin(9600, SERIAL_8N1, 16, 5);
  pinMode(p, OUTPUT);
  //pinMode(RST, OUTPUT);

  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskPMS
    ,  "TaskPMS"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}
