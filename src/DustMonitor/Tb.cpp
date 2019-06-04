#include<Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ThingsBoard.h>
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
//#define WIFI_AP_NAME        "FlyFly"
//#define WIFI_PASSWORD       "flyuntildie"
#define TOKEN               "mushroom1"
#define THINGSBOARD_SERVER  "192.168.100.202"
//#define THINGSBOARD_SERVER  "demo.thingsboard.io"
#define SERIAL_DEBUG_BAUD    115200
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
int led_delay;

WiFiClient espClient;
PubSubClient client(espClient);
static ThingsBoard tb(espClient);
int status = WL_IDLE_STATUS;
int quant = 20;
int send_delay = 2000;
int send_passed = 0;
bool subscribed = false;
RPC_Response processDelayChange(const RPC_Data &data)
{
  Serial.println("Received the set delay RPC method");
  led_delay = data;
  Serial.print("Set new delay: ");
  Serial.println(led_delay);
  return RPC_Response(NULL, led_delay);
}
RPC_Response processGetDelay(const RPC_Data &data)
{
  Serial.println("Received the get value method");
}
RPC_Response processSetGpioState(const RPC_Data &data)
{
  Serial.println("Received the set GPIO RPC method");

  int pin = data["pin"];
  bool enabled = data["enabled"];
  return RPC_Response(data["pin"], (bool)data["enabled"]);
}
RPC_Callback callbacks[] = {
  { "setValue",         processDelayChange },
  { "getValue",         processGetDelay },
  { "setGpioStatus",    processSetGpioState },
};
void TaskTb(void *pvParameters)
{
  (void) pvParameters;

  for (;;)
  {
    vTaskDelay(quant);
    send_passed += quant;
    if (!subscribed) {
      Serial.println("Subscribing for RPC...");
      if (!tb.RPC_Subscribe(callbacks, COUNT_OF(callbacks))) {
        Serial.println("Failed to subscribe for RPC");
        return;
      }
      Serial.println("Subscribe done");
      subscribed = true;
    }
//    if (send_passed > send_delay) {
//      Serial.println("Sending data...");
//      tb.sendTelemetryFloat("temperature", random(10, 60));
//      tb.sendTelemetryFloat("humidity", random(10, 100));
//      tb.
//      send_passed = 0;
//    }
    tb.loop();
  }
}
void TbSetup() {
  
  //client.setServer( THINGSBOARD_SERVER , 8080 );
  while (tb.connected() == false) {

    Serial.print(".");
    tb.connect(THINGSBOARD_SERVER, TOKEN);
    vTaskDelay(1000);
  }

  Serial.println("ThingsBoard Connected.");
  
  xTaskCreatePinnedToCore(
    TaskTb
    ,  "TaskTb"
    ,  4096
    ,  NULL
    ,  2
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);
}
