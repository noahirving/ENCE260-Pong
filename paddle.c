#include "system.h"
#include "pio.h"

#define PADDLE_LENGTH 3
#define BOARD_WIDTH LEDMAT_ROWS_NUM
#define MIN_POSITION 0
#define MAX_POSITION (BOARD_WIDTH - PADDLE_LENGTH)

static uint8_t paddle_position = 2;

uint8_t get_paddle (void)
{
    uint8_t paddle = 0;
    for (uint8_t i = 0; i < PADDLE_LENGTH; i++) {
        paddle |= (1 << (paddle_position + i));
    }

    return paddle;
}

void paddle_shift_left () {
    if (paddle_position < MAX_POSITION) {
        paddle_position++;
    }
}

void paddle_shift_right () {
    if (paddle_position > MIN_POSITION) {
        paddle_position--;
   }
}
