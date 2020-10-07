#include "paddle.h"

static uint8_t ball_col;
static uint8_t ball_row;

int check_ball_hit (void)
{
    uint8_t paddle = get_paddle;
    return (ball_row & (paddle & (1 << ball_row)));
}
