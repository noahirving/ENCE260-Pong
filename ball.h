#ifndef BALL_H
#define BALL_H

#include "paddle.h"
#include "system.h"

uint8_t check_ball_hit (void);

void ball_init (void);

void ball_update (void);

void ball_set_vector (int vector);

uint8_t get_ball (void);

uint8_t get_ball_column (void);

#endif
