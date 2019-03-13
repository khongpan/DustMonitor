#include "Blink.h"
#include "Network.h"
#include "DustSensor.h"

void HTTPConnect();
// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  BlinkSetup();
  NetworkSetup();
  DustSensorSetup(); 
  HTTPConnect();
}


void loop()
{
  //vTaskDelay(2000);
}
