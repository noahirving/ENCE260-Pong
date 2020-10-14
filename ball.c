/** @file   ball.c
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Ball object and methods.
*/

#include "paddle.h"
#include "system.h"
#include "ball.h"
#include "pio.h"
#include "ir_uart.h"
#include "ledmat.h"
#include "pacer.h"
#include "timer.h"


#define DEFAULT_Y_DIRECTION 1
#define NUM_FLASHES 4
#define FLASH_PERIOD 60
#define NUM_DIRECTIONS 7

/** Direction vectors for ball. */
static const Vector directions[] = {
    {-2, 1},
    {-2, 2},
    {-1, 2},
    {0, 2},
    {1, 2},
    {2, 2},
    {2, 1}
};


/** Returns a new ball.
 * @param direction_vector index of the direction vector the ball will be using.
 * @param position pointer to the position of the ball
 * @param speed speed of the ball. */
Ball new_ball (uint8_t direction_vector, Vector *position, uint8_t speed)
{
    // Scales the position of the ball.
    position->x = position->x * SCALER;
    position->y = position->y * SCALER;

    // Creates a ball with the given params and a default y_direction of one and speed_increase = false
    Ball ball = {direction_vector, DEFAULT_Y_DIRECTION, position, speed, false};
    return ball;
}


/** Inverts the x direction of the ball.
 * @param self the ball. */
void invert_x_direction (Ball *self)
{
    self->direction_vector = (NUM_DIRECTIONS - 1) - self->direction_vector;
}


/** Returns true if the ball is transferable.
 * @param self the ball. */
bool ball_is_transferable (Ball *self)
{
    // If the ball is beyond the bottom of the grid and its y direction is down.
    return ball_get_position(self).y  < MIN_Y && self->y_direction < 0;
}


Vector ball_get_position (Ball *self)
{
    Vector position = {self->position->x / SCALER, self->position->y / SCALER};
    return position;
}

/** Returns if the message is an encoded ball.
 * @param message possible encoded ball. */
bool is_ball (char message)
{
    // Message is ball if 8th bit is '0'
    return ~(message >> 7);
}


/** Returns if the ball can collide with the paddle.
 * @param self the ball. */
bool can_collide (Ball *self)
{
    // If the ball has the same y position as the paddle.
    return ball_get_position(self).y >= MAX_Y; // TODO; Change to paddle position
}


/** Gets the position of the ball as a bit pattern.
 * @param self the ball. */
static uint8_t ball_get_pattern (Ball *self)
{
    return (1 << ball_get_position(self).x);
}


/** Returns if the ball is colliding with the paddle.
 * @param self the ball
 * @param paddle_pattern. */
bool is_colliding (Ball *self,  uint8_t paddle_pattern)
{
    // If the ball's pattern and paddle's pattern cross.
    return ball_get_pattern(self) & paddle_pattern;
}


/** Gets the ball's direction vector.
 * @param self the ball. */
static Vector ball_get_direction (Ball *self)
{
    Vector direction;
    // Sets the direction from the ball's direction vector and y_direction
    direction.x = directions[self->direction_vector].x;
    direction.y = directions[self->direction_vector].y * self->y_direction;

    return direction;
}


/** Bounces the ball off the paddle.
 * @param self the ball. */
void ball_bounce_paddle (Ball *self)
{
    Vector direction = ball_get_direction (self);

    /* Returns ball to position before collision with paddle,
     * so that it does no phase through paddle when colliding. */
    self->position->x -= direction.x * self->speed;
    self->position->y -= direction.y * self->speed;

    // Reverses y_direction.
    self->y_direction = -self->y_direction;

    int8_t num = (timer_get () % 3) - 1;
    if (self->direction_vector == 0 && num == -1) {
        self->direction_vector = 0;
    } else if (self->direction_vector == NUM_DIRECTIONS - 1 && num == 1) {
        self->direction_vector = NUM_DIRECTIONS - 1;
    } else {
        self->direction_vector += num;
    }

    // Updates the ball position such that it has bounced off the paddle.
    ball_update_position (self);
}


/** Bounces ball off wall if on wall.
 * @param self the ball. */
void ball_bounce_wall (Ball *self)
{
    Vector position = ball_get_position(self);
    // If on walls or beyond walls, sets position to wall and inverts x direction.
    if (position.x <= MIN_X) {
        self->position->x = MIN_X * SCALER;
        invert_x_direction (self);

    } else if (position.x >= MAX_X) {
        self->position->x = MAX_X * SCALER;
        invert_x_direction (self);
    }
}


/** Updates the balls position.
 * @param self the ball. */
void ball_update_position (Ball *self)
{
    Vector direction = ball_get_direction (self);
    self->position->x += direction.x * self->speed;
    self->position->y += direction.y * self->speed;
}




/** Updates the ledmat to display the ball's current postition
 * @param Address to the ball object */
void ball_update_display (Ball* self)
{
    ledmat_display_column (ball_get_pattern(self), ball_get_position(self).y);
}


/** Increases the ball's speed up until the max defined speed
 * @param Address to the bal object */
void ball_increase_speed (Ball* self)
{
    if (self->speed < BALL_MAX_SPEED) {
        self->speed++;
        self->speed_increased = true;
    }
}


/** Flashes the ball on and off at its current position
 * @param Address of the ball object */
void flash_ball (Ball *self)
{
    uint16_t pacer_counter = 0;
    uint8_t flash_count = 0;
    while (flash_count < NUM_FLASHES) {
        pacer_wait ();
        paddle_update_display ();
        pacer_wait ();

        if (pacer_counter < FLASH_PERIOD / 2) {
            ball_update_display (self);
        }

        if (pacer_counter == FLASH_PERIOD) {
            flash_count++;
            pacer_counter = 0;
        }
        pacer_counter++;
    }
}
