#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "config.h"
#define LED_BUILTIN_1 4
char str_date[256];
char str_time [256];
String  str, str1, S;
struct tm tmstruct ;
const int buttonpin = 0;
int buttonstate = 0; int N ;
static int Start = 0;
static int enterKey = 0;

/*********************************Task*********************************/
void TaskNetwork(void *pvParameters)
{
  (void) pvParameters;
  int lost = 0;
  int lostt = 0;
  int lost_config = 0;
  
  for (;;)
  {
    if (WiFi.status() != WL_CONNECTED) {
      lost++;
      if (lost == 10) ESP.restart();
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.begin();
    } else if (WiFi.status() == WL_CONNECTED) {
      lost = 0;
    }
    
    pinMode(buttonpin,INPUT_PULLUP);
    buttonstate = digitalRead(buttonpin);
    if (buttonstate == LOW) {
      lostt++;
      Serial.println(lostt);
    } else lostt = 0;
    if (lostt > 30) {
      Serial.println("Enter....Smartconfig");
      WiFi.mode(WIFI_AP_STA);
      WiFi.beginSmartConfig();
      Serial.println("Waiting for SmartConfig.");
      while (!WiFi.smartConfigDone())  {
        digitalWrite(LED_BUILTIN_1, HIGH); 
       
      }

      Serial.println("SmartConfig done.");
      config_set_str("SmartConfig", "yes");
      WiFi.begin();
       
   }
     
    vTaskDelay(100);

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
void NetworkSetup() {
  pinMode(buttonpin, INPUT_PULLUP);

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
  int wait = 0;
  while (WiFi.status() != WL_CONNECTED) {
    wait++;
    if (wait > 20) ESP.restart();
    vTaskDelay(1000);
    Serial.print(".");
  }
  Serial.print("IP Address: ");
  Serial.println("WiFi Connected.");
  Serial.println(WiFi.localIP());
  Serial.println("Contacting Time Server");
  //configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  configTime(3600 * timezone, daysavetime * 3600, "clock.nectec.or.th", "0.pool.ntp.org", "1.pool.ntp.org");              //Set configTime NTP
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
    ,  "TaskNetwork"
    ,  1024
    ,  NULL
    ,  2
    ,  NULL );
}
