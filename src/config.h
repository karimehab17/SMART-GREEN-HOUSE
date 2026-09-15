#ifndef CONFIG_H
#define CONFIG_H

#include "../MCAL/ADC/ADC_interface.h"

/* Sensor ADC channels configuration */
#define SENSOR_TEMP_CHANNEL   ADC_CHANNEL_0
#define SENSOR_SOIL_CHANNEL   ADC_CHANNEL_1
#define SENSOR_LIGHT_CHANNEL  ADC_CHANNEL_2

#define SENSOR_ADC_REFERENCE  ADC_REF_AVCC
#define SENSOR_ADC_PRESCALER  ADC_PRESC_64

#endif /* CONFIG_H */