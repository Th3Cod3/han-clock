#include <stddef.h>
#include "stepper.h"
#include "hall.h"
#include "pit.h"

#define DIR_PIN 20
#define CHANGE_DIR_CCW GPIOE->PCOR = (1 << DIR_PIN)
#define CHANGE_DIR_CW GPIOE->PSOR = (1 << DIR_PIN)
#define TICK_RECOVERY_TIME 50

uint16_t count;
uint32_t steppers_timestamp;
uint8_t stepper_on;
uint8_t steppers_mode = STEPPER_UNKNOWN_MODE;

uint32_t add_seconds = 0;
uint32_t add_k_seconds = 0;
uint32_t add_m_seconds = 0;
uint32_t add_g_seconds = 0;

stepper_t stepper_motors[STEPPER_MOTOR_INSTANCES] = {
	{21, SIM_SCGC5_PORTE_MASK, &PORTE->PCR[21], GPIOE},
	{22, SIM_SCGC5_PORTE_MASK, &PORTE->PCR[22], GPIOE},
	{23, SIM_SCGC5_PORTE_MASK, &PORTE->PCR[23], GPIOE},
	{29, SIM_SCGC5_PORTE_MASK, &PORTE->PCR[29], GPIOE}};

inline void stepper_init(void)
{
	// Setup common direction pin
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	PORTE->PCR[DIR_PIN] = PORT_PCR_MUX(1);
	GPIOE->PDDR |= (1 << DIR_PIN);

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

bool stepper_tick(void)
{
	static uint32_t next_tick_timeout = 0;

	if (!wait_millis(next_tick_timeout))
	{
		return false;
	}

	next_tick_timeout = millis + TICK_RECOVERY_TIME;
	switch (stepper_on)
	{
	case STEPPER_SECONDS:
	case STEPPER_K_SECONDS:
	case STEPPER_M_SECONDS:
	case STEPPER_G_SECONDS:
		stepper_motors[stepper_on].GPIO->PSOR = MASK(stepper_motors[stepper_on].pin);
		for (int i = 0; i < 100; i++)
		{
			__asm("nop");
		}
		stepper_motors[stepper_on].GPIO->PCOR = MASK(stepper_motors[stepper_on].pin);
		break;

	default:
		stepper_on = STEPPER_NO_STEPPER;
	}

	return true;
}

void stepper_running(void)
{
}

bool calibrate(void)
{
	CHANGE_DIR_CCW;
	if (hall_state(stepper_on))
	{
		stepper_on++;
	}

	if (stepper_on == STEPPER_MOTOR_INSTANCES)
	{
		stepper_on = STEPPER_NO_STEPPER;
		CHANGE_DIR_CW;
		steppers_mode = STEPPER_SYNC_MODE;
		return true;
	}

	stepper_tick();
	return false;
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

bool sync(void)
{
	if (add_seconds)
	{
		stepper_on = STEPPER_SECONDS;
		if (stepper_tick())
		{
			add_seconds--;
		}
	}
	else if (add_k_seconds)
	{
		stepper_on = STEPPER_K_SECONDS;
		if (stepper_tick())
		{
			add_k_seconds--;
		}
	}
	else if (add_m_seconds)
	{
		stepper_on = STEPPER_M_SECONDS;
		if (stepper_tick())
		{
			add_m_seconds--;
		}
	}
	else if (add_g_seconds)
	{
		stepper_on = STEPPER_G_SECONDS;
		if (stepper_tick())
		{
			add_g_seconds--;
		}
	}
	else {
		return true;
	}

	return false;
}

/**
 * @param current_time
 * @param seconds second to add
 */
void add_time(uint32_t current_time, uint32_t seconds)
{
	// calculating how many ticks each stepper needs
	add_seconds = (current_time % 1000) + (seconds % 1000);
	add_k_seconds = (current_time % 1000000) / 1000 + (seconds % 1000000) / 1000;
	add_m_seconds = (current_time % 1000000000) / 1000000 + (seconds % 1000000000) / 1000000;
	add_g_seconds = (current_time % 1000000000000) / 1000000000 + (seconds % 1000000000000) / 1000000000;

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
}

void set_clock(uint32_t seconds)
{
	set_stepper_to(STEPPER_SECONDS, seconds % 1000);
	set_stepper_to(STEPPER_K_SECONDS, (seconds % 1000000) / 1000);
	set_stepper_to(STEPPER_M_SECONDS, (seconds % 1000000000) / 1000000);
	set_stepper_to(STEPPER_G_SECONDS, (seconds % 1000000000000) / 1000000000);
}
