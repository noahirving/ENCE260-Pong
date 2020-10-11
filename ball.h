#ifndef BALL_H
#define BALL_H

#include "paddle.h"
#include "system.h"

typedef struct {
    int8_t x;
    int8_t y;
} Vector;

typedef struct {
    uint8_t direction_vector;
    int8_t y_direction;
    Vector *position;
    uint8_t speed;
} Ball;

Ball new_ball(uint8_t direction_vector, Vector *position, uint8_t speed);

/** Checks if the ball has hit the paddle. Occurs when the ball is in the
 * column ahead of the paddle
 * @return 1 if the ball hits the paddle otherwise 0 */
uint8_t check_ball_hit (Ball *self);

bool can_collide (Ball *self);

bool is_colliding (Ball *self,  uint8_t paddle_bitmap);

void ball_bounce_paddle (Ball *self);

void ball_bounce_wall (Ball * self);

/** Updates the balls position based on its direction */
void ball_update_position (Ball *self);

bool ball_is_transferable (Ball *self);
/** Transfers the ball to the opponent's screen */
void transfer_ball (Ball *self);

/** Receives the ball from the opponent's screen */
Ball receive_ball (char message);

/** Sets the direction of the ball */
void ball_set_vector (int vector);

/** Gets the position of the ball as a bit pattern */
uint8_t get_ball (Ball *self);

/** Gets the column of the ball */
uint8_t get_ball_column (Ball *self);

#endif
