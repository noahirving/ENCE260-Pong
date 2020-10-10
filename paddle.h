#ifndef PADDLE_H
#define PADDLE_H

#include "system.h"

uint8_t get_paddle (void);
void paddle_set_period (uint16_t period);
void paddle_update (void);

#endif
