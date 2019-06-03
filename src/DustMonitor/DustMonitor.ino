#include "Network.h"
#include "DustSensor.h"
#include "CCS811.h"
#include "HDC1080.h"
#include "RotaryKey.h"
#include "Blink.h"
#include "config.h"
#include "PMS.h"
#include "HTTP.h"
#include "SHT31.h"
#include"Blynk.h"
#include"Tb.h"
#include <Wire.h>
void setup() {
  pinMode (19, OUTPUT);
  digitalWrite(19, HIGH);
  Serial.begin(115200);
  Wire.begin(21, 22, 10000);
  // ConfigSetup();
  // BlinkSetup();
  // NetworkSetup();
  //BlynkSetup();
  //TbSetup();
  // DustSensorSetup();
  //CCS811Setup();
  // SHT31Setup();
  // HDC1080Setup();
  RKSetup();
  // HTTPSetup();
}
void loop()
{

}
