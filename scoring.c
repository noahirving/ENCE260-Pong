#include "system.h"
#include "tinygl.h"
#include "pacer.h"

#define WIN_SCORE 3
#define SCORE_DISPLAY_TIMER 3500

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


/** Displays the score after the round has ended */
void display_score (void)
{
    /* Format of the score display. '#' indicates the number score of each player */
    char score[] = {' ', ' ', '#', '-', '#', ' ', ' '};
    score[2] = get_your_score() + '0';
    score[4] = get_opponent_score() + '0';

    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_clear ();
    tinygl_text (score);

    uint16_t pacer_count = 0;

    while (pacer_count < SCORE_DISPLAY_TIMER) {
        pacer_wait ();
        tinygl_update ();
        pacer_count++;
    }
}


/** Checks if a received message is score related by checking the largest bit (control bit)
 * @param message The received message
 * @return true if the control bit is 1, otherwise false */
bool is_score (char message)
{
    if (message >> 7 == 1) {
        return true;
    } else {
        return false;
    }
}
