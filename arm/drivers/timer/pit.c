#include "pit.h"

#define PIT_MS_TO_LOADVAL(ms) ((DEFAULT_SYSTEM_CLOCK/2/1000)*ms)
#define LED_PIN 9

uint32_t millis = 0;
uint32_t timestamp = 1717978726; // 2024-04-20 20:18:46

inline bool wait_millis(uint32_t ms)
{
    return millis > ms;
}

void pit_init(void)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT(1);

    PIT->MCR = PIT_MCR_MDIS(0);
    PIT->CHANNEL[0].LDVAL = PIT_MS_TO_LOADVAL(1);
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE(1) | PIT_TCTRL_TEN(1);

    NVIC_ClearPendingIRQ(PIT_IRQn);
    NVIC_EnableIRQ(PIT_IRQn);
}

void PIT_IRQHandler(void)
{
    NVIC_DisableIRQ(PIT_IRQn);
    millis++;
    if (millis % 1000 == 0)
    {
        timestamp++;
    }

    PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF(1);
    NVIC_EnableIRQ(PIT_IRQn);
}
