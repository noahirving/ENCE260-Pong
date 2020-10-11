#ifndef SCORING_H
#define SCORING_H

#include "system.h"

/** Increments your score by 1 */
void increase_your_score (void);


/** Increments opponent's score by 1 */
void increase_opponent_score (void);


/** Gets your score
 * @return your score */
uint8_t get_your_score (void);


/** Gets your opponent's score
 * @return your opponent's score */
uint8_t get_opponent_score (void);


/** Checks if the game is finished by seeing if the win score has been reached
 * @return 1 if the game has finsihed, otherwise 0 */
bool game_finished (void);


/** Checks if your opponent has won the game
 * @return 1 if your opponent won, othwerwise 0 */
bool opponent_won (void);

#endif
