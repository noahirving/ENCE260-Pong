#ifndef BALL_H
#define BALL_H

#include "paddle.h"
#include "system.h"

/* Structure of a Vector. */
typedef struct {
    int8_t x;
    int8_t y;
} Vector;

/*Structure of a ball. */
typedef struct {
    uint8_t direction_vector;
    int8_t y_direction;
    Vector *position;
    uint8_t speed;
} Ball;

/** Returns a new ball.
 * @param direction_vector index of the direction vector the ball will be using.
 * @param position pointer to the position of the ball
 * @param speed speed of the ball. */
Ball new_ball(uint8_t direction_vector, Vector *position, uint8_t speed);

/** Checks if the ball has hit the paddle. Occurs when the ball is in the
 * column ahead of the paddle
 * @return 1 if the ball hits the paddle otherwise 0 */
uint8_t check_ball_hit (Ball *self);

/** Returns if the ball can collide with the paddle.
 * @param self the ball. */
bool can_collide (Ball *self);

/** Returns if the ball is colliding with the paddle.
 * @param self the ball
 * @param paddle_bitmap. */
bool is_colliding (Ball *self,  uint8_t paddle_bitmap);

/** Bounces the ball off the paddle.
 * @param self the ball. */
void ball_bounce_paddle (Ball *self);

/** Bounces ball off wall if on wall.
 * @param self the ball. */
void ball_bounce_wall (Ball * self);

/** Updates the balls position.
 * @param self the ball. */
void ball_update_position (Ball *self);

/** Returns true if the ball is transferable.
 * @param self the ball. */
bool ball_is_transferable (Ball *self);

/** Transfers the ball to the opponent's screen.
 * @param self the ball. */
void transfer_ball (Ball *self);

/** Receives the ball from the opponent's screen.
 * @param ball the ball to set
 * @param message encoded ball. */
void receive_ball (Ball *ball, char message);

/** Gets the position of the ball as a bit pattern.
 * @param self the ball. */
uint8_t get_ball (Ball *self);

/** Gets the column of the ball.
 * @param self the ball. */
uint8_t get_ball_column (Ball *self);

#endif
