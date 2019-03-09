#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H
void DustSensorSetup();  // This is a task.
void DustSensorRead(int *PM2_5, int *PM10);
#endif
