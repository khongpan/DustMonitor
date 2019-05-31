#include "Arduino.h"
#include "hpma115S0.h"
extern "C" {
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
}


HPMA115S0::HPMA115S0(Stream& serial):
  _serial(serial)
{
  _serial.setTimeout(100);
}

void HPMA115S0::Init() {
  //Serial.println("PS- Initializing...");
  delay(100);
  StartParticleMeasurement();
  delay(100);
  DisableAutoSend();
}


void HPMA115S0::SendCmd(uint8_t * cmdBuf, unsigned int cmdSize) {
  
  while (_serial.available())
    _serial.read();

  //Send command
  //Serial.print("PS- Sending cmd: ");
  unsigned int index = 0;
  for (index = 0; index < cmdSize; index++) {
    //Serial.print(cmdBuf[index], HEX);
    //Serial.print(" ");
    _serial.write(cmdBuf[index]);
  }
  //Serial.println("");
  return;
}
int HPMA115S0::ReadCmdResp(char * dataBuf, unsigned int dataBufSize, unsigned int cmdType) {
  static unsigned char respBuf[HPM_MAX_RESP_SIZE];
  static unsigned int respIdx = 0;
  static unsigned int calChecksum = 0;

  respIdx = 0;
  calChecksum = 0;
  memset(respBuf, 0, sizeof(respBuf));
  _serial.setTimeout(100);
  //Serial.println("PS- Waiting for cmd resp...");
  if (_serial.readStringUntil(HPM_CMD_RESP_HEAD)) {
    delay(1); 
    respBuf[HPM_HEAD_IDX] = HPM_CMD_RESP_HEAD;
    respBuf[HPM_LEN_IDX] = _serial.read(); 

    
    if (respBuf[HPM_LEN_IDX] && ((respBuf[HPM_LEN_IDX] + 1) <=  sizeof(respBuf) - 2) && (respBuf[HPM_LEN_IDX] - 1) <= dataBufSize ) {
      if (_serial.readBytes(&respBuf[HPM_CMD_IDX], respBuf[HPM_LEN_IDX] + 1) == (respBuf[HPM_LEN_IDX] + 1)) { //read respBuf[HPM_LEN_IDX] num of bytes + calChecksum byte
        if (respBuf[HPM_CMD_IDX] == cmdType) { 

          
          for (respIdx = 0; respIdx < (2 + respBuf[HPM_LEN_IDX]); respIdx++) {
            calChecksum += respBuf[respIdx];
          }
          calChecksum = (65536 - calChecksum) % 256;
          if (calChecksum == respBuf[2 + respBuf[HPM_LEN_IDX]]) {
            //Serial.println("PS- Received valid data!!!");
            memset(dataBuf, 0, dataBufSize);
            memcpy(dataBuf, &respBuf[HPM_DATA_START_IDX], respBuf[HPM_LEN_IDX] - 1);
            return (respBuf[HPM_LEN_IDX] - 1);
          }
        }
      }
    }
  }
  return false;
}
boolean HPMA115S0::ReadParticleMeasurement(unsigned int * pm2_5, unsigned int * pm10) {
  uint8_t cmdBuf[] = {0x68, 0x01, 0x04, 0x93};
  char dataBuf[HPM_READ_PARTICLE_MEASURMENT_LEN - 1];

  //Serial.println("PS- Reading Particle Measurements..." );

  
  SendCmd(cmdBuf, 4);

  
  if (ReadCmdResp(dataBuf, sizeof(dataBuf), READ_PARTICLE_MEASURMENT) == (HPM_READ_PARTICLE_MEASURMENT_LEN - 1)) {
    _pm2_5 = dataBuf[0] * 256 + dataBuf[1];
    _pm10 = dataBuf[2] * 256 + dataBuf[3];
    *pm2_5 = _pm2_5;
    *pm10 = _pm10;
    // Serial.println("PS- PM 2.5: " + String(_pm2_5) + " ug/m3" );
    // Serial.println("PS- PM 10: " + String(_pm10) + " ug/m3" );
    return true;
  }
  return false;
}
void HPMA115S0::StartParticleMeasurement() {
  uint8_t cmd[] = {0x68, 0x01, 0x01, 0x96};
  SendCmd(cmd, 4);
}

void HPMA115S0::StopParticleMeasurement() {
  uint8_t cmd[] = {0x68, 0x01, 0x02, 0x95};
  SendCmd(cmd, 4);
}
void HPMA115S0::EnableAutoSend() {
  uint8_t cmd[] = {0x68, 0x01, 0x40, 0x57};
  SendCmd(cmd, 4);
}
void HPMA115S0::DisableAutoSend() {
  uint8_t cmd[] = {0x68, 0x01, 0x20, 0x77};
  SendCmd(cmd, 4);
}
unsigned int HPMA115S0::GetPM2_5() {
  return _pm2_5;
}
unsigned int HPMA115S0::GetPM10() {
  return _pm10;
}
