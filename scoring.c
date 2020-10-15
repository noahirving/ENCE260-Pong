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



/** Checks if your opponent has won the game
 * @return 1 if your opponent won, othwerwise 0 */
static bool opponent_won (void)
{
    return (opponent_score == WIN_SCORE);
}



/** Checks if the game is finished by seeing if the win score has been reached
 * @return 1 if the game has finsihed, otherwise 0 */
bool game_finished (void)
{
    return (your_score == WIN_SCORE || opponent_won ());
}



/** Displays the score. */
void display_score (void)
{
    // Format of the score display. '#' is a placeholder for the score of each player.
    char score[] = "  #-#  ";
    // Adds scores to score string in correct char range.
    score[2] = your_score + '0';
    score[4] = opponent_score + '0';

    // Displays the score.
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_clear ();
    tinygl_text (score);

    for (uint16_t counter = 0; counter < SCORE_DISPLAY_TIMER; counter++) {
        pacer_wait ();
        tinygl_update ();
    }
}



/** Checks who won the game and displays corresponding message */
void end_game (void)
{
    // Leading spaces in text for formatting.
    if (opponent_won ()) {
        tinygl_text ("  LOSER");
    } else {
        tinygl_text ("  WINNER");
    }
}
