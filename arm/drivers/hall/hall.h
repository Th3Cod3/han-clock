#ifndef HALL_H
#define HALL_H

#include <stdbool.h>
#include <MKL25Z4.h>

#define HALL_SENSOR_INSTANCES 4

typedef struct
{
    uint8_t pin;
    uint32_t scgc5_mask;
    volatile uint32_t *PCR;
    GPIO_Type *GPIO;
} hall_t;

extern hall_t hall_sensors[HALL_SENSOR_INSTANCES];

void hall_init(void);
bool hall_state(uint8_t sensor);

#endif