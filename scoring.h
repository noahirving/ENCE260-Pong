#ifndef SCORING_H
#define SCORING_H

#include "system.h"

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


/** Checks if a received message is score related by checking the largest bit (control bit)
 * @param message The received message
 * @return true if the control bit is 1, otherwise false */
bool is_score (char message);


/** Checks who won the game and displays corresponding message */
void end_game (void);

#endif
