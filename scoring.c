#include "system.h"

#define WIN_SCORE 3

static int your_score = 0;
static int opponent_score = 0;

void increase_your_score (void)
{
    your_score++;
}

void increase_opponent_score (void)
{
    opponent_score++;
}

bool check_game_over (void) {
    if (your_score == WIN_SCORE) {
        // you win
        return 1
    } else if (opponent_score == WIN_SCORE) {
        // opponent win
        return 1
    }
    return 0
}
