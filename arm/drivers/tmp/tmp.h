#ifndef TMP_H
#define TMP_H

#include <stdint.h>
#include <MKL25Z4.h>

void tmp_init(void);
float tmp_sample(void);

#endif
