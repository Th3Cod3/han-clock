#ifndef STEPPER_H
#define STEPPER_H
#include <MKL25Z4.h>

#define MASK(x) (1UL << (x))

#define STEPPER_MOTOR_INSTANCES 4

#define STEPPER_SECONDS 0
#define STEPPER_K_SECONDS 1
#define STEPPER_M_SECONDS 2
#define STEPPER_G_SECONDS 3
#define STEPPER_NO_STEPPER STEPPER_MOTOR_INSTANCES

/**
 * @brief this struct is used to drive the stepper motors steps.
 * For direction we use a common pin for all the motors
 */
typedef struct
{
    uint8_t pin;
    uint32_t scgc5_mask;
    __IO uint32_t *PCR;
    GPIO_Type *GPIO;
} stepper_t;

extern uint16_t count;
extern uint8_t stepper_on;

extern stepper_t stepper_motors[STEPPER_MOTOR_INSTANCES];

void stepper_init(void);
void calibrate(void);
void sync(void);
void set_stepper_to(uint32_t stepper, uint32_t steps);
void set_clock(uint32_t seconds);
void add_time(uint32_t current_time, uint32_t add_seconds);
#endif // STEPPER_H