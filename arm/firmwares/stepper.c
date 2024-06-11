#include <stepper.h>
#include <stdio.h> 

uint64_t volatile Time = 15055005055;
int volatile Stepper_On;
int volatile count;

void PIT_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(PIT_IRQn);
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)
	{ // 1s timer
		PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
			Time++;
			//GPIOE->PTOR = MASK(22);
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
//PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disabling 1 Sec counter
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
		//Calibrate();
	uint64_t Set_Time = 0;
	uint64_t Missed_Seconds = 0;
 do{
	 Missed_Seconds = Time - Set_Time;
	 Set_Time = Time;
		Add_time(Set_Time, Missed_Seconds);
	 
 }
 while(!(Set_Time == Time));
		
}

void Add_time(uint64_t Current_time, uint64_t Add_seconds){
	//calculating how many ticks each stepper needs
	int Add_Seconds = (Current_time % 1000) + (Add_seconds % 1000);
	int Add_K_Seconds = (Current_time %1000000)/1000 + (Add_seconds %1000000)/1000;
	int Add_M_Seconds = (Current_time %1000000000)/1000000 + (Add_seconds %1000000000)/1000000;
	int Add_G_Seconds = (Current_time %1000000000000)/1000000000 + (Add_seconds %1000000000000)/1000000000;
	
	//when the count is higher than 1000 at each stepper make sure to add 1 to the following
	if(Add_Seconds >= 1000){
		Add_K_Seconds = Add_K_Seconds + Add_Seconds/1000;
	}
	Add_Seconds = Add_Seconds-(Current_time % 1000);
	if(Add_K_Seconds >= 1000){
		Add_M_Seconds = Add_M_Seconds + Add_K_Seconds/1000;
	}
	Add_K_Seconds = Add_K_Seconds - (Current_time %1000000)/1000;
	if(Add_M_Seconds >= 1000){
		Add_G_Seconds = Add_G_Seconds + Add_M_Seconds/1000;
	}
	Add_M_Seconds = Add_M_Seconds - (Current_time %1000000000)/1000000 ;
	Add_G_Seconds = Add_G_Seconds - (Current_time %1000000000000)/1000000000 ;
	
	//setting the steppers
	Set_Stepper_To(STEPPER_SECONDS,Add_Seconds);
	Set_Stepper_To(STEPPER_K_SECONDS,Add_K_Seconds);
	Set_Stepper_To(STEPPER_M_SECONDS,Add_M_Seconds);
	Set_Stepper_To(STEPPER_G_SECONDS,Add_G_Seconds);
}

void Set_Clock(uint64_t Seconds)
{
		Set_Stepper_To(STEPPER_SECONDS,Seconds % 1000);
		Set_Stepper_To(STEPPER_K_SECONDS,(Seconds%1000000)/1000);
		Set_Stepper_To(STEPPER_M_SECONDS,(Seconds%1000000000)/1000000);
		Set_Stepper_To(STEPPER_G_SECONDS,(Seconds%1000000000000)/1000000000);
}
