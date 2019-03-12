#include "Blink.h"
#include "Network.h"
#include "DustSensor.h"
#include "HTTP.h"

//#include "Display.h"
// the setup function runs once when you press reset or power the board

void setup() {

  Serial.begin(115200);
  // initialize serial communication at 115200 bits per second:
  // DisplaySetup();
  // BlinkSetup();
   NetworkSetup();
//   HTTPSetup();
//   HTTPConnect();
  // DustSensorSetup();
  // Display();
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.



  
 
}

int i=0;
void loop()
{
      struct tm tmstruct ;
      
    delay(1000);
    tmstruct.tm_year = 0;
    getLocalTime(&tmstruct, 5000);
    Serial.printf("\n%d Now is : %d-%02d-%02d %02d:%02d:%02d\n",i,(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
    i++;
  



}
