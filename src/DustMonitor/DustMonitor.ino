#include "Blink.h"
#include "Network.h"
#include "DustSensor.h"
#include "CCS811.h"

void HTTPConnect();
// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  BlinkSetup();
  //NetworkSetup();
  //DustSensorSetup(); 
  CCS811Setup();
  //HTTPConnect();
}


void loop()
{
  //vTaskDelay(2000);
}
