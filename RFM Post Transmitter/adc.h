#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <stdbool.h>
void adc_init(void);
bool adc_read_result(unsigned int *res);
void adc_sensor_read(unsigned int channel);

extern bool adc_result_ready;
#endif
