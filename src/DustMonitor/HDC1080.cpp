#include "Wire.h"
#include "HDC1080.h"

HDC1080JS::HDC1080JS(){

}

void HDC1080JS::config(){

  temperatureRaw=0;
  humidityRaw=0;

  //config the temp sensor to read temp then humidity in one transaction
  //config the resolution to 14 bits for temp & humidity

  writeRegister(0x02,0x10);

}

void HDC1080JS::writeRegister(uint8_t address, uint16_t value){
  Wire.beginTransmission(ADDR);
  Wire.write(address);
  Wire.write(value);
  Wire.endTransmission();
}



void HDC1080JS::readTempHumid(){
  //set pointer register
  Wire.beginTransmission(ADDR);
  Wire.write(0x00);
  Wire.endTransmission();
  vTaskDelay(100);
  Wire.requestFrom(ADDR, 4, true);
  temperatureRaw = temperatureRaw << 8 | Wire.read();
  temperatureRaw = temperatureRaw << 8 | Wire.read();
  humidityRaw = humidityRaw << 8 | Wire.read();
  humidityRaw = humidityRaw << 8 | Wire.read();

}

//returns temp in celcius
float HDC1080JS::getTemp(){

  // (rawTemp/2^16)*165 - 40
  return ( (float)temperatureRaw )*165/65536 - 40;

}

float HDC1080JS::getRelativeHumidity(){

  //(rawHumidity/2^16)*100
  return ( (float)humidityRaw )*100/65536;
}

float* HDC1080JS::getTempHumid(float* tempHumid){

  *tempHumid = getTemp();
  *(tempHumid+1) = getRelativeHumidity();
  return tempHumid;

}



HDC1080JS tempsensor;



void TaskHDC1080(void *pvParameters) {

  for(;;) {
    
      tempsensor.readTempHumid();
      float temp = tempsensor.getTemp();
      float humid = tempsensor.getRelativeHumidity();
    
      Serial.print("T=");
      Serial.print(temp);
      Serial.print("C, RH=");
      Serial.print(humid);
      Serial.println("%");
      vTaskDelay(1000);
  } 
 
}

void HDC1080Setup() {
  Wire.begin();
  //Wire.setClock(400000); //set clock speed for I2C bus to maximum allowed for HDC1080
  //Serial.begin(9600);  // start serial for output

  //tempsensor = HDC1080JS();
  tempsensor.config();
    xTaskCreate(
    TaskHDC1080
    ,  "TaskHDC1080"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
}
