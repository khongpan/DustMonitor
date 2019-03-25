#include <Arduino.h>
#include <WiFi.h>
//#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "DustSensor.h"
//#include "esp_task_wdt.h"
#include "Network.h"
#include "PMS.h"
#include "CCS811.h"
#define USE_SERIAL Serial
 #include "HDC1080.h"
//WiFiMulti wifiMulti;
int V1, V2, PM2_5, PM10, Pm2, Pm1;
String URL;
String date_str;
int value1, value2;
String  time_str, DATETIME ;
String A = "http://agritronics.nstda.or.th/webpost0606/log.php?data1=DUST_A00001";
//String B = "2";  //เปลี่ยน chanal
String C = ",1000,A,";
String D = ",7,10,";
uint16_t eco2, etvoc, errstat, raw;
/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
HTTPClient http;
void TaskHTTP(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  struct tm t;


  for (;;) // A Task shall never return or exit.
  {

    vTaskDelay(950);
    getLocalTime(&t, 5000);

    if (((t.tm_min % 1 != 0) || (t.tm_sec != 0))) continue;

    Datetime(&DATETIME);
    ccs811.read(&eco2,&etvoc,&errstat,&raw);
    DustSensorRead(&V1, &V2);
    PMSValue(&Pm2, &Pm1);
    date_str = DATETIME;
    value1 = V1;
    value2 = V2;
      tempsensor.readTempHumid();
      float temp = tempsensor.getTemp();
      float humid = tempsensor.getRelativeHumidity();
    URL = String (A)  + String(C) + String (date_str)  + String (D) + int (value1) + "," + int (value2) + "," + int (Pm1) + "," + int(Pm2) 
    + "," + int (eco2) + "," + int (etvoc) + "," + float (temp)+","+float (humid);
  //URL ="http://agritronics.nstda.or.th/webpost0606/log.php?data1=DUST_A00001,1000,A,19/03/22,12:00:00,7,10,100,200,300,400";
    


    Serial.println(URL);
    Serial.println();
    //Serial.println(date_str);

    if ((WiFi.status() == WL_CONNECTED)) {

      USE_SERIAL.print("[HTTP] begin...\n");
      http.begin(URL);
      USE_SERIAL.print("[HTTP] GET...\n");
      int httpCode = http.GET();
      // USE_SERIAL.print(httpCode);
      if (httpCode > 0) {

        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          USE_SERIAL.println(payload);
        }
      } else {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }

    // vTaskDelay(300000);
  }
}
void HTTPSetup() {
  //USE_SERIAL.begin(115200);
  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  //    for(uint8_t t = 10; t > 0; t--) {
  //        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
  //        USE_SERIAL.flush();
  //        vTaskDelay(1000);
  //        //esp_task_wdt_reset();
  //    }

  // wifiMulti.addAP("FlyFly", "flyuntildie");

  xTaskCreate(
    TaskHTTP
    ,  "TaskHTTP"   // A name just for humans
    ,  8192 // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );


}
