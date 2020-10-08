#include "system.h"
#include "pio.h"
#include "navswitch.h"

#define PADDLE_LENGTH 3
#define BOARD_WIDTH LEDMAT_ROWS_NUM
#define MIN_POSITION 0
#define MAX_POSITION (BOARD_WIDTH - PADDLE_LENGTH)
#define LEFT NAVSWITCH_SOUTH
#define RIGHT NAVSWITCH_NORTH
static uint8_t paddle_position = (BOARD_WIDTH / 2) - (PADDLE_LENGTH / 2) + MIN_POSITION;

uint8_t get_paddle (void)
{
    uint8_t paddle = 0;
    for (uint8_t i = 0; i < PADDLE_LENGTH; i++) {
        paddle |= (1 << (paddle_position + i));
    }

    return paddle;
}

void paddle_shift_left (void)
{
    if (paddle_position < MAX_POSITION) {
        paddle_position++;
    }
}

void paddle_shift_right (void)
{
    if (paddle_position > MIN_POSITION) {
        paddle_position--;
   }
}

uint16_t paddle_counter = 0;
uint16_t move_period = 0;

void paddle_set_period (uint16_t period)
{
    if (move_period == 0 || period < move_period) {
        move_period = period;
        paddle_counter = move_period;
    } else {
        move_period = period;
    }
}

void paddle_update (void)
{

    if (paddle_counter >= move_period) {
        if (navswitch_down_p (LEFT)) {
            paddle_shift_left ();
            paddle_counter = 0;
        } else if (navswitch_down_p (RIGHT)) {
            paddle_shift_right ();
            paddle_counter = 0;
        }

    } else {
        paddle_counter++;
    }

}
