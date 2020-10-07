#ifndef PADDLE_H
#define PADDLE_H

#include "system.h"
#include "pio.h"

uint8_t get_paddle (void);

void paddle_update (void);

void paddle_init (void);

void paddle_shift_left (void);

void paddle_shift_right (void);

#endif
