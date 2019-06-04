#include <Arduino.h>
#include <WiFi.h>
//#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "DustSensor.h"
//#include "esp_task_wdt.h"
#include "Network.h"
#include "PMS.h"
#include "CCS811.h"
#include "SHT31.h"
#define USE_SERIAL Serial
#include "HDC1080.h"
#include "ThingsBoard.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include"Tb.h"
#include "BlynkSimpleEsp32.h"
//WiFiMulti wifiMulti;
int hpma_value_pm10, hpma_value_pm2_5, pms_value_PM2_5, pms_value_PM10, pms_value_Pm2_5, pms_value_Pm10, hpma_value_PM2_5, hpma_value_PM10;
String URL;
String date_time_str;

String   date_time_now ;
String head_url_str = "http://agritronics.nstda.or.th/webpost0606/log.php?data1=DUST_A00000";
//String chanal = "2";  //เปลี่ยน chanal
String  url_2_str = ",1000,A,";
String url_4_str = ",7,10,";
uint16_t eco2, etvoc, errstat, raw;
float sht31_temp, sht31_humid;

void Datetime(String *date) {
  struct tm tmstruct ;
  char str_date_value[16];
  char str_time_value[16];
  String  str_date, str_time, str_date_time_value;

  getLocalTime(&tmstruct, 5000);
  sprintf(str_date_value, "%d/%02d/%02d", (tmstruct.tm_year) - 100, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday);
  sprintf(str_time_value, "%02d:%02d:%02d", tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  str_date = str_date_value ;
  str_time = str_time_value;
  str_date_time_value = (str_date + "," + str_time);
  *date  =  str_date_time_value ;
  Serial.println (*date);
}
/*---------------------- Tasks ---------------------*/
extern WiFiClient espClient;
extern ThingsBoard tb(espClient);

HTTPClient http;
void TaskHTTP(void *pvParameters)
{
  (void) pvParameters;

  struct tm t;
  int old_eco2 = 400;
  int old_voc = 10;
  float temp, humid;
  int count = 0;
  for (;;)
  {
    vTaskDelay(950);
    getLocalTime(&t, 5000);

    if (((t.tm_min % 1 != 0) || (t.tm_sec != 0))) continue;

    Datetime(&date_time_now);
    int cnt = 0;
    /*********************/
    do {
      cnt++;
      ccs811.read(&eco2, &etvoc, &errstat, &raw);
      if (errstat != CCS811_ERRSTAT_OK) {
        eco2 = old_eco2;
        etvoc = old_voc;
        vTaskDelay(1000);
      }
    } while ((errstat != CCS811_ERRSTAT_OK ) && (cnt <= 11));
    old_eco2 = eco2;
    old_voc = etvoc;
    /********************************
        ccs811.read(&eco2, &etvoc, &errstat, &raw);
        if (eco2 == 400)||(etvoc == 0){
           cnt++
          if (cnt > 50 ){
            ccs811.read(&eco2, &etvoc, &errstat, &raw);
          }
        }
      /*******************************/
    tempsensor.readTempHumid();
    float  temp = tempsensor.getTemp();
    float humid = tempsensor.getRelativeHumidity();
    float Temp, Humid;
    if (temp == 125) {
      tempsensor.readTempHumid();
      temp = tempsensor.getTemp();
      if (temp == 125) {
        count++;
      } if (count == 4) {
        temp = tempsensor.getTemp();
        float Temp = temp;
      } else count = 0;
    }
    if (humid == 100) {
      tempsensor.readTempHumid();
      humid = tempsensor.getRelativeHumidity();
      if (humid == 100) {
        count++;
      } if (count == 4) {
        humid = tempsensor.getRelativeHumidity();
        float Humid = humid;
      } else count = 0;
    }
    if (temp == 125 ) {
      temp = Temp ;
    }
    if (humid == 100 ) {
      humid = Humid;
    }
    //    while (temp == 125) {
    //      tempsensor.readTempHumid();
    //      temp = tempsensor.getTemp();
    //    }
    //    while (humid == 100) {
    //      tempsensor.readTempHumid();
    //      humid = tempsensor.getRelativeHumidity();
    //    }
    Serial.print("T=");
    Serial.print(temp);
    Serial.print("C, RH=");
    Serial.print(humid);
    Serial.println("%");
    

    SHT31Read(&sht31_temp, &sht31_temp);
    DustSensorRead(&hpma_value_PM2_5, &hpma_value_PM10);
    //PMSValue(&pms_value_PM2_5, &pms_value_PM10);
    
    //tb.sendTelemetryFloat("temperature", sht31_temp);
    //tb.sendTelemetryFloat("humid", sht31_humid);
    tb.sendTelemetryFloat("temperature", temp);
    tb.sendTelemetryFloat("humid", humid);
    //tb.sendTelemetryFloat("temperature", 25);
    //tb.sendTelemetryFloat("humid", 50);
    Blynk.virtualWrite(V1, sht31_temp);
    Blynk.virtualWrite(V2, sht31_humid);
    Blynk.virtualWrite(V3, temp);
    Blynk.virtualWrite(V4, humid);
    
    date_time_str = date_time_now;
    URL = String (head_url_str)  + String(url_2_str) + String (date_time_str)  + String (url_4_str) + int (hpma_value_PM2_5) + "," + int (hpma_value_PM10) + "," + float (temp) + "," + float(humid)
          + "," + int (eco2) + "," + int (etvoc) + "," + float (sht31_temp) + "," + float (sht31_humid);
    //URL ="http://agritronics.nstda.or.th/webpost0606/log.php?data1=DUST_A00003,1000,A,19/03/22,12:00:00,7,10,100,200,300,400";
    Serial.println(URL);
    Serial.println();
    if ((WiFi.status() == WL_CONNECTED)) {
      USE_SERIAL.print("[HTTP] begin...\n");
      http.begin(URL);
      USE_SERIAL.print("[HTTP] GET...\n");
      int httpCode = http.GET();
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
  //   }
  xTaskCreate(
    TaskHTTP
    ,  "TaskHTTP"
    ,  8192
    ,  NULL
    ,  2
    ,  NULL );
}
