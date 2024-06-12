#include <MKL25Z4.h>
#include <stdio.h>

void tmp_init(void)
{
    // Enable clock to ADC0
    SIM->SCGC6 |= SIM_SCGC6_ADC0(1);
    
    // Enable clock to PORTs
    SIM->SCGC5 |= SIM_SCGC5_PORTB(1);
    
    // The input of the TMP is connectd to PTB0. Configure the pin as
    // ADC input pin (channel 8).
    PORTB->PCR[0] &= ~0x7FF;
    
    // Configure ADC
    // - ADLPC = 1 : Low-power configuration. The power is reduced at the 
    //               expense of maximum clock speed.
    // - ADIV[1:0] = 00 : The divide ratio is 1 and the clock rate is input 
    //                    clock.
    // - ADLSMP = 1 : Long sample time.
    // - MODE[1:0] = 11 : Single-ended 16-bit conversion
    // - ADICLK[1:0] = 01 : (Bus clock)/2
    ADC0->CFG1 = 0x9D;
}


/*!
 * \brief Samples the IR proximity sensor
 *
 * Take one samples from the IR proximity sensor.
 *
 * \return  The complemented 16-bit ADC value
 */
float tmp_sample(void)
{		
	


		float MAX_VALUE = 65535.0;
	
    // Start a conversion on channel 8
    ADC0->SC1[0] = 8;
    
    // Wait for conversion to complete
    while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
    {}
        
    // Read the result
    uint16_t res = (uint16_t)ADC0->R[0];
		float resV = res / MAX_VALUE * 3.3f;
		float resC = (resV - 0.51f) * 100.0f;
		
        
    // Complement the result, because the voltage falls with increasing IR level
    // and we want the result to rise with increasing IR level.
    return (resC);
}
