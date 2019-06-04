#include <Arduino.h>
#include "FreeRTOS.h"
//#include <tasks.h>

const byte interruptPin = 34;
volatile int rotationCounter = 0;
volatile int direction;

const byte clkPin =34;
const byte dirPin =35;
const byte btnPin =32;
const byte vccPin =33;
const byte gndPin =25;

 
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

unsigned long last_time=0;
unsigned long cur_time=0;
 
void IRAM_ATTR rkInterrupt() {

  cur_time=millis();
  if (cur_time-last_time<10) return;
  
  direction=digitalRead(dirPin);
  taskENTER_CRITICAL_ISR(&mux);

  if (direction==0) 
  { 
      rotationCounter++;

  } else {
      rotationCounter--;
  }
  taskEXIT_CRITICAL_ISR(&mux);
  
  last_time=cur_time;
 
}


int RKgetDial()
{
  int dial_step;
  taskENTER_CRITICAL(&mux);
  dial_step=rotationCounter;
  rotationCounter=0;
  taskEXIT_CRITICAL(&mux);
  return(dial_step);
}

int RKisDial()
{
  return rotationCounter;
}
 
void TaskRK(void *pvParameters) {

int prevRC=0;

 (void) pvParameters;

  for(;;) {
    if(RKisDial()!=0)
      Serial.println(RKgetDial());

/*    
    if(prevRC!=rotationCounter) {
      prevRC=rotationCounter;
      Serial.println(prevRC);
    }
*/    
     vTaskDelay(100);
  }
}


void RKSetup() {
 

  pinMode(dirPin, INPUT_PULLUP);
  pinMode(gndPin, OUTPUT);
  pinMode(vccPin, OUTPUT);
  digitalWrite(vccPin,HIGH);
  digitalWrite(gndPin,LOW);
  pinMode(clkPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(clkPin), rkInterrupt, RISING);

    xTaskCreate(
    TaskRK
    ,  "TaskRK"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
 
 
}
