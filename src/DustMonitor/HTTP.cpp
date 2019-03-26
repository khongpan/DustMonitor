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


void Datetime(String *DATE) {
  struct tm tmstruct ;
  char str_date[16];
  char str_time [16];
  String  str, str1, S;

  getLocalTime(&tmstruct, 5000);
  sprintf(str_date, "%d/%02d/%02d", (tmstruct.tm_year) - 100, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday);
  sprintf(str_time, "%02d:%02d:%02d", tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  str = str_date ;
  str1 = str_time;
  S = (str + "," + str1);
  *DATE  =  S ;
  Serial.println (*DATE);
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
HTTPClient http;
void TaskHTTP(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  struct tm t;
  int old_eco2 = 400;

  for (;;) // A Task shall never return or exit.
  {

    vTaskDelay(950);
    getLocalTime(&t, 5000);

    if (((t.tm_min % 1 != 0) || (t.tm_sec != 0))) continue;

    Datetime(&DATETIME);
    int cnt = 0;
    do {

      cnt++;
      ccs811.read(&eco2, &etvoc, &errstat, &raw);
      if (errstat != CCS811_ERRSTAT_OK) {
        eco2 = old_eco2;
        vTaskDelay(1000);
      }

    } while ((errstat != CCS811_ERRSTAT_OK )&& (cnt <= 11));

      old_eco2 = eco2;
    DustSensorRead(&V1, &V2);
    PMSValue(&Pm2, &Pm1);
    date_str = DATETIME;
    value1 = V1;
    value2 = V2;
    tempsensor.readTempHumid();
    float temp = tempsensor.getTemp();
    float humid = tempsensor.getRelativeHumidity();
    URL = String (A)  + String(C) + String (date_str)  + String (D) + int (value1) + "," + int (value2) + "," + int (Pm1) + "," + int(Pm2)
          + "," + int (eco2) + "," + int (etvoc) + "," + float (temp) + "," + float (humid);
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
