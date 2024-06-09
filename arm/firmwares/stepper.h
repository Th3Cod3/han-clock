#include <MKL25Z4.h>

#define MASK(x) 		(1UL <<(x))

#define STEPPER_SECONDS 	 1
#define STEPPER_K_SECONDS	 2
#define STEPPER_M_SECONDS  3
#define STEPPER_G_SECONDS  4
#define STEPPER_NO_STEPPER 5

extern volatile int count;

extern volatile int Stepper_On;
 
extern uint32_t Time;

void PIT_IRQHandler(void);
 
void Calibrate(void);
 
void Sync(void);
 
void Set_Stepper_To(int Stepper, unsigned int Steps);

void Set_Clock(uint64_t Minutes);
