
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
#include <stepper.h>


#define STEPPER_SECONDS 	 1
#define STEPPER_K_SECONDS	 2
#define STEPPER_M_SECONDS  3
#define STEPPER_G_SECONDS  4
#define STEPPER_NO_STEPPER 5

//int Stepper_On = STEPPER_NO_STEPPER;

//#define MASK(x) (1UL << (x))
// Local function prototypes
// static void delay_us(uint32_t d);
/*!
 * \brief Main application
 */
/*
void PIT_IRQHandler(void)
{
	//static int count = 0;
	NVIC_ClearPendingIRQ(PIT_IRQn);
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)
	{ // 1s timer
		PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
		
			GPIOE->PTOR = MASK(29);
	}
	else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) // 100m timer
	{
		PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;

			GPIOE->PTOR = MASK(29);
		switch (count)
		{                                                                    
		case 1:
			GPIOE->PTOR = MASK(21);
			break;

		case 2:
			GPIOE->PTOR = MASK(22);
			break;

		case 3:
			GPIOE->PTOR = MASK(23);
			break;

		case 4:
			GPIOE->PTOR = MASK(29);
			count = 0;
			break;
		default:
			count = 0;
			break;
		}
		count++;
	}
}*/

void Calibrate(void);
void Sync(void);
int main(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

	PORTE->PCR[20] = PORT_PCR_MUX(0x1);
	PORTE->PCR[21] = PORT_PCR_MUX(0x1);
	PORTE->PCR[22] = PORT_PCR_MUX(0x1);
	PORTE->PCR[23] = PORT_PCR_MUX(0x1);
	PORTE->PCR[29] = PORT_PCR_MUX(0x1);
	

	PORTA->PCR[4] = PORT_PCR_MUX(0x1);
	PORTA->PCR[5] = PORT_PCR_MUX(0x1);
	PORTA->PCR[12] = PORT_PCR_MUX(0x1);
	
	PORTD->PCR[4] = PORT_PCR_MUX(0x1);

	GPIOE->PDDR |= (1 << 20) | (1 << 21) | (1 << 22) | (1 << 23) | (1 << 29);
	GPIOA ->PDDR &= ~(MASK(4) | MASK(5) | MASK(12));
	GPIOD ->PDDR &= ~MASK(4);	
	// PORTD -> PCR[4] = PORT_PCR_MUX(1);	//Step Steppermoter
	// PORTE -> PCR[0] = PORT_PCR_MUX(1)|PORT_PCR_PS(1)|PORT_PCR_PE(1);// Calibrate sensor
	// PORTC -> PCR[9] = PORT_PCR_MUX(1);	//Direction Stepper
	// GPIOC ->PDDR |= MASK(9) | MASK(30);	//Output
	// GPIOD ->PDDR |= MASK(4);	//Output

	// Enable clock to PIT modele
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;

	// 0.5 s timer
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(12e6 - 1); // Genrate an event evrey second
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_CHN_MASK;	 // Disable chaining
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;	 // Enable interups

	// 100ms timer
	PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV((12e6 / 10) - 1); // Genrate an event evrey second
	PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_CHN_MASK;			 // Disable chaining
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;			 // Enable interups

	// Enable interups
	NVIC_SetPriority(PIT_IRQn, 1);
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);

	// Enable counters
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	
  Stepper_On = STEPPER_NO_STEPPER;
	count = 0;

	// GPIOC_PSOR= MASK(9);

	Sync();
	while (1)
	{
		/*
		if(!(PTD->PDIR & MASK(4))){
	Calibrate();
	
		}*/
	}
}
