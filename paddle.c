#include "system.h"

#define PADDLE_LENGTH 3
#define BOARD_WIDTH 7
#define MIN_POSITION 0
#define MAX_POSITION (BOARD_WIDTH - PADDLE_LENGTH)

static uint8_t paddle_position = 0;

static uint8_t get_paddle (void)
{
    uint8_t paddle = 0;
    for (uint8_t i = 0; i < PADDLE_LENGTH; i++) {
        paddle |= (1 << (paddle_position + i));
    }

    return paddle << 2;
}

