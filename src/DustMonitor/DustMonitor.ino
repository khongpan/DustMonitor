#include "Blink.h"
#include "Network.h"
#include "DustSensor.h"
#include "Display.h"
// the setup function runs once when you press reset or power the board

void setup() {
 
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  DisplaySetup();
  BlinkSetup();
//  NetWorkSetup();
  DustSensorSetup();
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}


void loop()
{
//  Serial.print("aaaa");// Empty. Things are done in Tasks.
//  vTaskDelay(2000);
     
         void TaskDisplay(void *pvParameters);
        
}
