#include <Arduino.h>
#include <WiFi.h>
#include <Time.h>

char* ssid     = "FlyFly";
char* password = "flyuntildie";
/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskNetwork(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
    vTaskDelay(1000);  // one tick delay (15ms) in between reads for stability
  }
}



// the setup function runs once when you press reset or power the board
void NetworkSetup() {
    long timezone = 7; 
    byte daysavetime = 0;

    Serial.begin(115200);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    //Serial.println(password);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Contacting Time Server");
    
    //configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
    configTime(3600*timezone, daysavetime*3600, "clock.nectec.or.th", "0.pool.ntp.org", "1.pool.ntp.org");
    struct tm tmstruct ;
    delay(2000);
    tmstruct.tm_year = 0;
    while(!getLocalTime(&tmstruct, 1000)) 
    { 
      Serial.print(".");
    }
    Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
//    xTaskCreate(
//      TaskNetwork
//      ,  "TaskNetwork"   // A name just for humans
//      ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
//      ,  NULL
//      ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
//      ,  NULL );

}
