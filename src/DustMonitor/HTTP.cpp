#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#define USE_SERIAL Serial
WiFiMulti wifiMulti;


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskHTTP(void *pvParameters)  // This is a task.
{
  (void) pvParameters;



  for (;;) // A Task shall never return or exit.
  {
        
  }
}
HTTPClient http;
// the setup function runs once when you press reset or power the board
void HTTPConnect(){
  
  if((WiFi.status()== WL_CONNECTED)) {

        
        USE_SERIAL.print("[HTTP] begin...\n");
        http.begin("http://agritronics.nstda.or.th/webpost0606/log.php?data1=DUST_A00002,1000,A,19/03/11,11:36:00,7,10,101,202"); 
        USE_SERIAL.print("[HTTP] GET...\n");
        int httpCode = http.GET();
        USE_SERIAL.print(httpCode);
        if(httpCode > 0) {
            
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

           
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(5000);
}
void HTTPSetup() {
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 10; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

   // wifiMulti.addAP("FlyFly", "flyuntildie");
 
    xTaskCreate(
    TaskHTTP
    ,  "TaskHTTP"   // A name just for humans
    ,  4096 // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
    

}
