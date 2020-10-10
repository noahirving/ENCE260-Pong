#include "system.h"

#define WIN_SCORE 3

static uint8_t your_score = 0;
static uint8_t opponent_score = 0;


/** Increments your score by 1 */
void increase_your_score (void)
{
    your_score++;
}

/** Increments opponent's score by 1 */
void increase_opponent_score (void)
{
    opponent_score++;
}


/** Gets your score
 * @return your score */
uint8_t get_your_score (void)
{
    return your_score;
}


/** Gets your opponent's score
 * @return your opponent's score */
uint8_t get_opponent_score (void)
{
    return opponent_score;
}


/** Checks if the game is finished by seeing if the win score has been reached
 * @return 1 if the game has finsihed, otherwise 0 */
bool game_finished (void)
{
    if (your_score == WIN_SCORE || opponent_score == WIN_SCORE) {
        return true;
    } else {
        return false;
    }
}


/** Checks if your opponent has won the game
 * @return 1 if your opponent won, othwerwise 0 */
bool opponent_won (void)
{
    if (opponent_score == WIN_SCORE) {
        return true;
    } else {
        return false;
    }
}

