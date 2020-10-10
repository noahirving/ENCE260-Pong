#ifndef PADDLE_H
#define PADDLE_H
#include "system.h"

void paddle_set_length (uint8_t length);
void paddle_set_period (uint16_t period);
void paddle_init (uint8_t new_length, uint16_t new_period);
void paddle_update (void);
uint8_t get_paddle (void);

#endif
