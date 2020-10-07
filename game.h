#ifndef GAME_H
#define GAME_H

#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "led.h"
#include "ir_uart.h"
#include "paddle.h"

#define PACER_RATE 500
#define COUNTDOWN_TIMER_RATE 500
#define MESSAGE_RATE 20

void display_countdown (char character);

void tinygl_setup (void);

void game_init (void);

uint8_t is_ready (void);

uint8_t opponent_is_ready (void);

void startup (void);

void wait_for (uint8_t (*func)(void));

#endif
