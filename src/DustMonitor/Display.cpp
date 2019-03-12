//#include "Arduino.h"
//#include "heltec.h"
//#include "DustSensor.h"
// const int buttonPin = 0; 
//int buttonState = 0; 
//
// 
//#if CONFIG_FREERTOS_UNICORE
//#define ARDUINO_RUNNING_CORE 0
//#else
//#define ARDUINO_RUNNING_CORE 1
//#endif
//String Value1 ; 
//String Value2 ; 
///*--------------------------------------------------*/
///*---------------------- Tasks ---------------------*/
///*--------------------------------------------------*/
//
//void TaskDisplay(void *pvParameters)  // This is a task.
//{
//  (void) pvParameters;
//
///*
//  Blink
//  Turns on an LED on for one second, then off for one second, repeatedly.
//    
//  If you want to know what pin the on-board LED is connected to on your ESP32 model, check
//  the Technical Specs of your board.
//*/
//
//  // initialize digital LED_BUILTIN on pin 13 as an output.
////  pinMode(LED_BUILTIN, OUTPUT);
//
//  for (;;) // A Task shall never return or exit.
//  {
//     
//  }
//}
//
//// the setup function runs once when you press reset or power the board
//void DisplaySetup() {
//   pinMode(buttonPin, INPUT);
//  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/);
//  Heltec.display->flipScreenVertically();
//  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
//  Heltec.display->setFont(ArialMT_Plain_16);
//  Heltec.display->clear();
//  Heltec.display->drawString(0, 0, "Waiting for \nconnection...");
//  Heltec.display->display();
//  Heltec.display->clear();
//  // Now set up two tasks to run independently.
//  xTaskCreatePinnedToCore(
//    TaskDisplay
//    ,  "TaskDisplay"   // A name just for humans
//    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
//    ,  NULL
//    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
//    ,  NULL 
//    ,  ARDUINO_RUNNING_CORE);
//
//  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
//}
//void Display(){
//
//buttonState = digitalRead(buttonPin);
//        if (buttonState == HIGH) {
//        
//        Heltec.display->clear();
//     //   sprintf(str,"%d-%02d-%02d",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday);
//      //  Heltec.display->setFont(ArialMT_Plain_10);
//     //   Heltec.display->drawString(0, 5, str);
//     //   sprintf(str,"%02d:%02d:%02d",tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
////        Heltec.display->drawString(70, 5, str);
//        Heltec.display->setFont(ArialMT_Plain_16);
//        Heltec.display->drawString(0, 27,"PM 2.5");
//        Heltec.display->setFont(ArialMT_Plain_24);
//        Heltec.display->drawString(56, 18,Value1);
//        Heltec.display->setFont(ArialMT_Plain_10);
//        Heltec.display->drawString(97, 27," ug/m3");
//        Heltec.display->setFont(ArialMT_Plain_16);
//        Heltec.display->drawString(0, 47,"PM 10");
//        Heltec.display->setFont(ArialMT_Plain_24);
//        Heltec.display->drawString(56,42,Value2);
//        Heltec.display->setFont(ArialMT_Plain_10);
//        Heltec.display->drawString(97,47," ug/m3");
//        Heltec.display->display();
//       // vTaskDelay(30000);  // one tick delay (15ms) in between reads for stability
//        }
//        else {
//       Heltec.display->clear();
//        }
//}
