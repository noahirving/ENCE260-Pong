#include "system.h"

#define WIN_SCORE 3

static uint8_t your_score = 0;
static uint8_t opponent_score = 0;

void increase_your_score (void)
{
    your_score++;
}

void increase_opponent_score (void)
{
    opponent_score++;
}

uint8_t get_your_score (void)
{
    return your_score;
}

uint8_t get_opponent_score (void)
{
    return opponent_score;
}


bool game_finished (void)
{
    if (your_score == WIN_SCORE || opponent_score == WIN_SCORE) {
        return true;
    } else {
        return false;
    }
}

bool opponent_won (void)
{
    if (opponent_score == WIN_SCORE) {
        return true;
    } else {
        return false;
    }
}

