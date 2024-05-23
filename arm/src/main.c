
	/*! ***************************************************************************
 *
 * \brief     Blinky
 * \file      main.c
 * \author    Hugo Arends
 * \date      June 2021
 *
 * \copyright 2021 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include <MKL25Z4.h>
#include <stdbool.h>

#define MASK(x) 		(1UL <<(x))
// Local function prototypes
//static void delay_us(uint32_t d);
/*!
 * \brief Main application
 */
 void Calibrate(void);
 
 void PIT_IRQHandler(void)
 {
		NVIC_ClearPendingIRQ(PIT_IRQn);
	 if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK){
	 PIT->CHANNEL[0].TFLG |=PIT_TFLG_TIF_MASK;
		 
        PTE->PTOR = MASK(1);
	 }
	 else if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK)
	 {
	 PIT->CHANNEL[1].TFLG |=PIT_TFLG_TIF_MASK;
		 PTE->PTOR = MASK(1);
	 }
	 
 } 
int main(void)
{

  SIM->SCGC5 |= SIM_SCGC5_PORTE(1) | SIM_SCGC5_PORTE(0);
		PORTE -> PCR[1] = PORT_PCR_MUX(1);	//Step Steppermoter
		PORTE -> PCR[0] = PORT_PCR_MUX(1)|PORT_PCR_PS(1)|PORT_PCR_PE(1);// Calibrate sensor
		PORTE -> PCR[2] = PORT_PCR_MUX(1);	//Direction Stepper
		GPIOE ->PDDR |= MASK(1) | MASK(2);	//Output
		GPIOE ->PDDR &= ~MASK(0);	//Input
	
		//Enable clock to PIT modele
		SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
		
		PIT->MCR &= ~PIT_MCR_MDIS_MASK;
		PIT->MCR  |= PIT_MCR_FRZ_MASK;
	
	//Second timer
		PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(12e6-1);	//Genrate an event evrey second
		PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_CHN_MASK;		//Disable chaining
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;		//Enable interups
	
	//100ms timer
	PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV((12e6/100)-1);	//Genrate an event evrey second
		PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_CHN_MASK;		//Disable chaining
		PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;		//Enable interups
	
		//Enable interups
		NVIC_SetPriority(PIT_IRQn,1);
		NVIC_ClearPendingIRQ(PIT_IRQn);
		NVIC_EnableIRQ(PIT_IRQn);
	
		//Enable counters
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
       
		PTE->PCOR = MASK(2);
	


	
    while(1)
    { 
			/*for(int i = 0; i < 10;i++){
        // Toggle the pin and wait
        PTE->PTOR = MASK(0);
        delay_us(20000);
        PTE->PTOR = MASK(0);
        delay_us(20000);
			}

        // Toggle the pin and wait
        PTE->PTOR = MASK(0);
        delay_us(300);
        PTE->PTOR = MASK(0);
        delay_us(300);*/
			
			if(!(PTE->PDIR & MASK(0))){
			Calibrate();
    }
}
		}

void Calibrate(void){
	
		while(!(PTE->PDIR & MASK(0))){}//Testfunction (release sensor)
	
	
		PTE->PSOR = MASK(2);//Change direction to clockwise
		PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disabling 1 Sec counter
		PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;//enabling 100ms counter
		while((PTE->PDIR & MASK(0))){}//wait for the home
			
		PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_TEN_MASK;//disabling  1s counter
		PTE->PCOR = MASK(2);//Change direction to counter clockwise
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;//disabling 100ms counter
		while(!(PTE->PDIR & MASK(0))){}//Testfunction (release sensor)
			
}
			
/*!
 * \brief Creates a blocking delay
 *
 * Software delay of approximately 1.02 us, depending on compiler version,
 * CPU clock frequency and optimizations.
 * - C compiler: ARMClang v6
 * - Language C: gnu11
 * - CPU clock: 48 MHz
 * - Optimization level: -O3
 * - Link-Time Optimization: disabled
 *
 * \param[in]  d  delay in microseconds
 *
static void delay_us(uint32_t d)
{

#if (CLOCK_SETUP != 1)
#warning This delay function does not work as designed
#endif

    volatile uint32_t t;

    for(t=4*d; t>0; t--)
    {
        __asm("nop");
        __asm("nop");
    }
}
*/