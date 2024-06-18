#include <stddef.h>
#include "stepper.h"
#include "hall.h"
#include "pit.h"

uint16_t count;
uint8_t stepper_on;
uint8_t steppers_mode = STEPPER_UNKNOWN_MODE;

stepper_t stepper_motors[STEPPER_MOTOR_INSTANCES] = {
	{21, SIM_SCGC5_PORTE_MASK, &PORTE->PCR[21], GPIOE},
	{22, SIM_SCGC5_PORTE_MASK, &PORTE->PCR[22], GPIOE},
	{23, SIM_SCGC5_PORTE_MASK, &PORTE->PCR[23], GPIOE},
	{29, SIM_SCGC5_PORTE_MASK, &PORTE->PCR[29], GPIOE}};

inline void stepper_init(void)
{
	// Setup common direction pin
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	PORTE->PCR[20] = PORT_PCR_MUX(1);
	GPIOE->PDDR |= (1 << 20);

	for (int i = 0; i < STEPPER_MOTOR_INSTANCES; i++)
	{
		if (stepper_motors[i].PCR != NULL)
		{
			continue;
		}

		SIM->SCGC5 |= stepper_motors[i].scgc5_mask;
		*(stepper_motors[i].PCR) = PORT_PCR_MUX(1);
		stepper_motors[i].GPIO->PDDR |= (1 << stepper_motors[i].pin);
	}
}

void stepper_tick(void)
{
	switch (stepper_on)
	{
	case STEPPER_SECONDS:
	case STEPPER_K_SECONDS:
	case STEPPER_M_SECONDS:
	case STEPPER_G_SECONDS:
		stepper_motors[stepper_on].GPIO->PTOR = MASK(stepper_motors[stepper_on].pin);
		break;

	default:
		stepper_on = STEPPER_NO_STEPPER;
		return;
	}
}

void calibrate(void)
{
	while (hall_state(STEPPER_SECONDS))
	{
	} // Testfunction (release sensor)

	GPIOE_PSOR = MASK(20); // Change direction to clockwise
	while ((PTD->PDIR & MASK(4)))
	{
	} // wait for the home

	GPIOE_PCOR = MASK(20); // Change direction to counter clockwise
	count = 0;			   // Restting count for set_stepper_to function
	while (!(PTD->PDIR & MASK(4)))
	{
	} // Testfunction (release sensor)
}

void set_stepper_to(uint32_t stepper, uint32_t steps)
{
	count = 0;
	stepper_on = stepper;

	while (!(steps * 2 <= count))
	{
	}

	stepper_on = STEPPER_NO_STEPPER;
}

void sync(void)
{
	uint32_t set_time = 0;
	uint32_t missed_seconds = 0;

	do
	{
		missed_seconds = timestamp - set_time;
		set_time = timestamp;
		add_time(set_time, missed_seconds);
	} while (!(set_time == timestamp));
}

/**
 * @param current_time 
 * @param seconds second to add
 */
void add_time(uint32_t current_time, uint32_t seconds)
{
	// calculating how many ticks each stepper needs
	uint32_t add_seconds = (current_time % 1000) + (seconds % 1000);
	uint32_t add_k_seconds = (current_time % 1000000) / 1000 + (seconds % 1000000) / 1000;
	uint32_t add_m_seconds = (current_time % 1000000000) / 1000000 + (seconds % 1000000000) / 1000000;
	uint32_t add_g_seconds = (current_time % 1000000000000) / 1000000000 + (seconds % 1000000000000) / 1000000000;

	// when the count is higher than 1000 at each stepper make sure to add 1 to the following
	if (add_seconds >= 1000)
	{
		add_k_seconds = add_k_seconds + add_seconds / 1000;
	}
	add_seconds = add_seconds - (current_time % 1000);
	if (add_k_seconds >= 1000)
	{
		add_m_seconds = add_m_seconds + add_k_seconds / 1000;
	}
	add_k_seconds = add_k_seconds - (current_time % 1000000) / 1000;
	if (add_m_seconds >= 1000)
	{
		add_g_seconds = add_g_seconds + add_m_seconds / 1000;
	}
	add_m_seconds = add_m_seconds - (current_time % 1000000000) / 1000000;
	add_g_seconds = add_g_seconds - (current_time % 1000000000000) / 1000000000;

	// setting the steppers
	set_stepper_to(STEPPER_SECONDS, add_seconds);
	set_stepper_to(STEPPER_K_SECONDS, add_k_seconds);
	set_stepper_to(STEPPER_M_SECONDS, add_m_seconds);
	set_stepper_to(STEPPER_G_SECONDS, add_g_seconds);
}

void set_clock(uint32_t seconds)
{
	set_stepper_to(STEPPER_SECONDS, seconds % 1000);
	set_stepper_to(STEPPER_K_SECONDS, (seconds % 1000000) / 1000);
	set_stepper_to(STEPPER_M_SECONDS, (seconds % 1000000000) / 1000000);
	set_stepper_to(STEPPER_G_SECONDS, (seconds % 1000000000000) / 1000000000);
}
