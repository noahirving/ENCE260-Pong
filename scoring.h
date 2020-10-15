/** @file   scoring.h
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Game scoring and win conditions.
*/

#ifndef SCORING_H
#define SCORING_H

#include "system.h"



/** Initiates the score for both players to 0 */
void scoring_init (void);



/** Increments opponent's score by 1 and notifies them that
 * the round is over */
void lost_round (void);



/** Increments your score by 1 */
void opponent_lost_round (void);



/** Checks if the game is finished by seeing if the win score has been reached
 * @return 1 if the game has finsihed, otherwise 0 */
bool game_finished (void);



/** Displays the score after the round has ended */
void display_score (void);



/** Checks who won the game and displays corresponding message */
void end_game (void);



#endif // SCORING_H
