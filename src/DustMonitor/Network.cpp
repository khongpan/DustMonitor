#include <Arduino.h>
#include <WiFi.h>
#include <Time.h>
#include "config.h"
char str_date[256];
char str_time [256];
String  str, str1, S;
struct tm tmstruct ;

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskNetwork(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  int lost=0;
  
  for (;;) // A Task shall never return or exit.
  {
    if (WiFi.status() != WL_CONNECTED) {

      lost++;

      if(lost==10) ESP.restart();

      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.begin();
    }else if(WiFi.status() == WL_CONNECTED){
      lost=0;
      
    }
    
   vTaskDelay(30000);  
  }
}

void NetworkSmartConfig() {
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    vTaskDelay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("SmartConfig done.");

  
}

// the setup function runs once when you press reset or power the board
void NetworkSetup() {
 
  long timezone = 7;
  byte daysavetime = 0;
  
  Serial.begin(115200);
  
  String str = config_get_str("SmartConfig", "no");
  if (str == "no") {

    NetworkSmartConfig();
    config_set_str("SmartConfig", "yes");

  }
  Serial.println("Waiting for WiFi");
  WiFi.begin();
  int wait=0;
  while (WiFi.status() != WL_CONNECTED) {
    wait++;
    if (wait>20) ESP.restart();
    
    vTaskDelay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Contacting Time Server");

  //configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  configTime(3600 * timezone, daysavetime * 3600, "clock.nectec.or.th", "0.pool.ntp.org", "1.pool.ntp.org");

  vTaskDelay(2000);
  tmstruct.tm_year = 0;
  Serial.print("OK");
  while (!getLocalTime(&tmstruct, 1000))
  {
    Serial.print(".");
  }

  Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct.tm_year) + 1900, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  xTaskCreate(
    TaskNetwork
    ,  "TaskNetwork"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

}
void Datetime(String *DATE) {
  getLocalTime(&tmstruct, 5000);
  sprintf(str_date, "%d/%02d/%02d", (tmstruct.tm_year) - 100, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday);
  sprintf(str_time, "%02d:%02d:%02d", tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  str = str_date ;
  str1 = str_time;
  S = (str + "," + str1);
  *DATE  =  S ;
  Serial.println (*DATE);
}
