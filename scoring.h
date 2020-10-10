#ifndef SCORING_H
#defing SCORING_H

#include "system.h"


void increase_your_score (void);

void increase_opponent_score (void);

uint8_t get_your_score (void);

uint8_t get_opponent_score (void);

bool game_finished (void);

bool opponent_won (void);

#endif
