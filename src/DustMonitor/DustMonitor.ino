#include "Network.h"
#include "DustSensor.h"
#include "CCS811.h"
#include "HDC1080.h"
#include "RotaryKey.h"
#include "Blink.h"
#include "config.h"
#include "PMS.h"
#include "HTTP.h"
void setup() {

  Serial.begin(115200);

  ConfigSetup();
  BlinkSetup();
  NetworkSetup();
  DustSensorSetup();
  PMSSetup();
  CCS811Setup();
  HDC1080Setup();
  //RKSetup();
  HTTPSetup();




}


void loop()
{

  //vTaskDelay(2000);

}
