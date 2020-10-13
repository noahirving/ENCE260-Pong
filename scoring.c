/** @file   scoring.c
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Game scoring and win conditions.
*/

#include "system.h"
#include "tinygl.h"
#include "pacer.h"
#include "ir_uart.h"
#include "navswitch.h"

#define WIN_SCORE 3
#define SCORE_DISPLAY_TIMER 1750
#define LOST (1 << 7)

static uint8_t your_score;
static uint8_t opponent_score;


/** Initiates the score for both players to 0 */
void scoring_init (void)
{
    your_score = 0;
    opponent_score = 0;
}


/** Increments opponent's score by 1 and notifies them that
 * the round is over */
void lost_round (void)
{
    opponent_score++;
    ir_uart_putc (LOST);
}


/** Increments your score by 1 */
void opponent_lost_round (void)
{
    your_score++;
}

/** Gets your score
 * @return your score */
static uint8_t get_your_score (void)
{
    return your_score;
}


/** Gets your opponent's score
 * @return your opponent's score */
static uint8_t get_opponent_score (void)
{
    return opponent_score;
}


/** Checks if the game is finished by seeing if the win score has been reached
 * @return 1 if the game has finsihed, otherwise 0 */
bool game_finished (void)
{
    return (your_score == WIN_SCORE || opponent_score == WIN_SCORE);
}


/** Checks if your opponent has won the game
 * @return 1 if your opponent won, othwerwise 0 */
static bool opponent_won (void)
{
    return (opponent_score == WIN_SCORE);
}


/** Displays the score after the round has ended */
void display_score (void)
{
    /* Format of the score display. '#' is a placeholder for the score of each player */
    char score[] = {' ', ' ', '#', '-', '#', ' ', ' '};
    score[2] = get_your_score () + '0';
    score[4] = get_opponent_score () + '0';

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
    return (message >> 7) & 1;
}


/** Checks who won the game and displays corresponding message */
void end_game (void)
{
    /* Leading spaces in text for formatting */
    if (opponent_won ()) {
        tinygl_text ("  LOSER");
    } else {
        tinygl_text ("  WINNER");
    }

    while (!navswitch_push_event_p (NAVSWITCH_PUSH)) {
        pacer_wait ();
        navswitch_update ();
        tinygl_update ();
    }
}
