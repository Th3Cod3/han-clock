#include <stepper.h>
#include <stdio.h> 

uint32_t Time = 54565;
int volatile Stepper_On;
int volatile count;

void PIT_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(PIT_IRQn);
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)
	{ // 1s timer
		PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
		
			GPIOE->PTOR = MASK(22);
	}
	else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) // 100m timer
	{

		PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;

		
		switch (Stepper_On)
		{
		case STEPPER_SECONDS:
			GPIOE->PTOR = MASK(21);
			break;

		case STEPPER_K_SECONDS:
			GPIOE->PTOR = MASK(22);
			break;

		case STEPPER_M_SECONDS:
			GPIOE->PTOR = MASK(23);
			break;

		case STEPPER_G_SECONDS:
			GPIOE->PTOR = MASK(29);
			break;
		case STEPPER_NO_STEPPER:
			count = 0;
			return;
		default:
			count = 0;
			Stepper_On = STEPPER_NO_STEPPER;
			return;
		}
		count++;
	}
}

void Calibrate(void){
	
		while(!(PTD->PDIR & MASK(4))){}//Testfunction (release sensor)
	
	  GPIOE_PSOR= MASK(20);//Change direction to clockwise
		PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disabling 1 Sec counter
		PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;//enabling 100ms counter
		while((PTD->PDIR & MASK(4))){}//wait for the home
			
		PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disabling  100ms counter
	 GPIOE_PCOR= MASK(20);//Change direction to counter clockwise
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;//enabeling  1s counter
			count = 0; // Restting count for Set_Stepper_To function
		while(!(PTD->PDIR & MASK(4))){}//Testfunction (release sensor)
			
}

void Set_Stepper_To(int Stepper, unsigned int Steps)
{
PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disabling 1 Sec counter
	count = 0;
			Stepper_On = Stepper;
	
		PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;//enabling 100ms counter
		while(!(Steps*2 <= count)){}
			Stepper_On = STEPPER_NO_STEPPER;
PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disabling 100ms counter
			
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;//enabeling  1s counter
}
void Sync(void)
{
		Calibrate();
		PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disabling 1 Sec counter
	int seconds = Time % 1000;
	Set_Stepper_To(STEPPER_G_SECONDS, seconds);
		
}

void Set_Clock(uint64_t Minutes)
{
		Set_Stepper_To(STEPPER_SECONDS,Minutes % 1000);
		Set_Stepper_To(STEPPER_K_SECONDS,(Minutes/1000) % 1000);
		Set_Stepper_To(STEPPER_M_SECONDS,(Minutes/1000000000) % 1000000);
		Set_Stepper_To(STEPPER_G_SECONDS,(Minutes/1000000000) % 1000000000);
}
