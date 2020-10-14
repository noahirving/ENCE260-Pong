/** @file   ball.h
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Ball object and methods.
*/

#ifndef BALL_H
#define BALL_H

#include "paddle.h"
#include "system.h"

#define SCALER 10
#define MIN_X 0
#define MAX_X (LEDMAT_ROWS_NUM - 1)
#define MIN_Y 0
#define MAX_Y (LEDMAT_COLS_NUM - 1)
#define BALL_MIN_SPEED 3
#define BALL_MAX_SPEED 6
#define DEFAULT_BALL_DIRECTION 3
#define DEFAULT_BALL_POSITION {3, 0}
#define BALL_SPEED_INC_PERIOD 3
#define BALL_UPDATE_PERIOD 40

/* Structure of a Vector. */
typedef struct {
    int8_t x;
    int8_t y;
} Vector;


/*Structure of a ball. */
typedef struct {
    int8_t direction_vector;
    int8_t y_direction;
    Vector *position;
    uint8_t speed;
    bool speed_increased;
} Ball;


/** Returns a new ball.
 * @param direction_vector index of the direction vector the ball will be using.
 * @param position pointer to the position of the ball
 * @param speed speed of the ball. */
Ball new_ball(uint8_t direction_vector, Vector* position, uint8_t speed);


/** Returns if the ball can collide with the paddle.
 * @param self Address of the ball. */
bool can_collide (Ball* self);


/** Returns if the ball is colliding with the paddle.
 * @param self Address of the ball
 * @param paddle_pattern Bit pattern representing the paddle */
bool is_colliding (Ball* self,  uint8_t paddle_bitmap);


/** Bounces the ball off the paddle.
 * @param self Address of the ball. */
void ball_bounce_paddle (Ball* self);


/** Bounces ball off wall if on wall.
 * @param self Address of the ball. */
void ball_bounce_wall (Ball* self);


/** Updates the balls position.
 * @param self Address of the ball. */
void ball_update_position (Ball* self);


/** Returns true if the ball is transferable.
 * @param self Address of the ball. */
bool ball_is_transferable (Ball* self);


/** Returns if the message is an encoded ball.
 * @param message possible encoded ball. */
bool is_ball (char message);


/** Increases the ball's speed up until the max defined speed
 * @param self Address to the bal object */
void ball_increase_speed (Ball* self);


/** Updates the ledmat to display the ball's current postition
 * @param self Address to the ball object */
void ball_update_display (Ball* self);


/** Flashes the ball on and off at its current position
 * @param self Address of the ball object */
void flash_ball (Ball* self);


/** Inverts the x direction of the ball.
 * @param self Address of the ball. */
void invert_x_direction (Ball* self);


/** Gets the position vecotr of the ball
 * @param self Address of the ball */
Vector ball_get_position (Ball* self);

#endif
