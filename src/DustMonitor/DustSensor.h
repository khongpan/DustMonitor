#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H
void DustSensorSetup();  
void DustSensorRead(int *PM2_5_value, int *PM10_value);
#endif
