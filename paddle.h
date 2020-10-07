#ifndef PADDLE_H
#define PADDLE_H

#include "system.h"
#include "pio.h"

#define PADDLE_LENGTH 3
#define BOARD_WIDTH 7
#define MIN_POSITION 0
#define MAX_POSITION (BOARD_WIDTH - PADDLE_LENGTH)

uint8_t get_paddle (void);

void paddle_update (void);

void paddle_init (void);

void paddle_shift_left (void);

void paddle_shift_right (void);

#endif
