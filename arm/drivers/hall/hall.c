#include "hall.h"

hall_t hall_sensors[HALL_SENSOR_INSTANCES] = {
    {4, SIM_SCGC5_PORTA_MASK, &PORTA->PCR[4], GPIOA},
    {5, SIM_SCGC5_PORTA_MASK, &PORTA->PCR[5], GPIOA},
    {12, SIM_SCGC5_PORTA_MASK, &PORTA->PCR[12], GPIOA},
    {4, SIM_SCGC5_PORTD_MASK, &PORTD->PCR[4], GPIOD}};

void hall_init(void)
{
    for (int i = 0; i < HALL_SENSOR_INSTANCES; i++)
    {
        SIM->SCGC5 |= hall_sensors[i].scgc5_mask;
        *(hall_sensors[i].PCR) = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
        hall_sensors[i].GPIO->PDDR &= ~(1 << hall_sensors[i].pin);
    }
}

/**
 * @brief return the state of a hall sensor 1 = active, 0 = not active
 * 
 * @param sensor the sensor number from 0 to HALL_SENSOR_INSTANCES
 */
bool hall_state(uint8_t sensor)
{
    if (sensor >= HALL_SENSOR_INSTANCES)
    {
        return false;
    }

    return hall_sensors[sensor].GPIO->PDIR & (1 << hall_sensors[sensor].pin) ? 1 : 0;
}