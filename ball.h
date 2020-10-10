#ifndef BALL_H
#define BALL_H

#include "paddle.h"
#include "system.h"

/** Checks if the ball has hit the paddle. Occurs when the ball is in the
 * column ahead of the paddle
 * @return 1 if the ball hits the paddle otherwise 0 */
uint8_t check_ball_hit (void);

/** Initializes the ball to be at the centre of the board */
void ball_init (void);

/** Updates the balls position based on its direction */
void ball_update (void);

/** Sets the direction of the ball */
void ball_set_vector (int vector);

/** Gets the position of the ball as a bit pattern */
uint8_t get_ball (void);

/** Gets the column of the ball */
uint8_t get_ball_column (void);

#endif
