#ifndef PIT_H
#define PIT_H

#include <MKL25Z4.h>
#include <stdbool.h>

extern uint32_t millis;
extern uint32_t timestamp;

void pit_init(void);
bool wait_millis(uint32_t ms);

#endif // PIT_H
