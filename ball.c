#include "paddle.h"
#include "system.h"
#include "ball.h"
#include "pio.h"
#include "ir_uart.h"
#include "ledmat.h"
#include "pacer.h"


#define SCALER 10
#define MIN_X (0 * SCALER)
#define MAX_X (6 * SCALER)
#define MIN_Y (0 * SCALER)
#define MAX_Y (4 * SCALER)
#define NUM_DIRECTIONS 7
#define DEFAULT_Y_DIRECTION 1
#define NUM_FLASHES 4
#define FLASH_PERIOD 60

/** Direction vectors for ball. */
static const Vector directions[] = {
    {-2, 1},
    {-1, 1},
    {-1, 2},
    {0, 1},
    {1, 2},
    {1, 1},
    {2, 1}
};

/** Returns a new ball.
 * @param direction_vector index of the direction vector the ball will be using.
 * @param position pointer to the position of the ball
 * @param speed speed of the ball. */
Ball new_ball(uint8_t direction_vector, Vector *position, uint8_t speed)
{
    // Scales the position of the ball.
    position->x = position->x * SCALER;
    position->y = position->y * SCALER;

    // Creates a ball with the given params and a default y_direction of one
    Ball ball = {direction_vector, DEFAULT_Y_DIRECTION, position, speed};
    return ball;
}

/** Inverts the x direction of the ball.
 * @param self the ball. */
static void invert_x_direction (Ball *self)
{
    self->direction_vector = (NUM_DIRECTIONS - 1) - self->direction_vector;
}

/** Returns true if the ball is transferable.
 * @param self the ball. */
bool ball_is_transferable (Ball *self)
{
    // If the ball is beyond the bottom of the grid and its y direction is down.
    return self->position->y  < MIN_Y && self->y_direction < 0;
}

/** Transfers the ball to the opponent's screen.
 * @param self the ball. */
void transfer_ball (Ball *self)
{
    char message;
    uint8_t direction_vector;
    // Inverts x position.
    /* Each variable must be divided seperately to avoid invalid position being sent.
     * i.e SCALER = 10, MIN_X = 0, MAX_X = 60, self->position->x = 55
     * position_x = 0 + 6 - 5 = 1
     * if scaled after
     * position_x = (0 + 60 - 55) / 10 = 0
     * invalid as the position is scaled down to grid (0 / 10 + 55 / 10 = 5)! */
    uint8_t position_x = MIN_X / SCALER + MAX_X / SCALER - self->position->x / SCALER;

    // Inverts direction for mirrored receiver
    invert_x_direction (self);
    direction_vector = self->direction_vector;

    // Encodes ball into char.
    message = (direction_vector << 3) | position_x;
    ir_uart_putc (message); // Transfer ball position and vector as single character
}

/** Receives the ball from the opponent's screen.
 * @param ball the ball to set
 * @param message encoded ball. */
void receive_ball (Ball *ball, char message)
{
    // Decodes x position (first 3 bits).
    uint8_t position_x = message & 0b111;
    // Decodes direction vector (next 3 bits).
    uint8_t direction_vector = (message >> 3) & 0b111;


    ball->position->x = position_x * SCALER;
    ball->position->y = MIN_Y * SCALER;
    ball->y_direction = DEFAULT_Y_DIRECTION;
    ball->direction_vector = direction_vector;
    ball->speed = 10; // TODO
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
    return self->position->y >= MAX_Y; // TODO; Change to paddle position
}

/** Returns if the ball is colliding with the paddle.
 * @param self the ball
 * @param paddle_bitmap. */
bool is_colliding (Ball *self,  uint8_t paddle_bitmap)
{
    // If the ball's bitmap and paddle's bitmap cross.
    return get_ball (self) & paddle_bitmap;
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

    // Updates the ball position
    ball_update_position (self);
}

/** Bounces ball off wall if on wall.
 * @param self the ball. */
void ball_bounce_wall (Ball *self)
{
    // If on walls or beyond walls, sets position to wall and inverts x direction.
    if (self->position->x <= MIN_X) {
        self->position->x = MIN_X;
        invert_x_direction (self);

    } else if (self->position->x >= MAX_X) {
        self->position->x = MAX_X;
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

/** Gets the position of the ball as a bit pattern.
 * @param self the ball. */
uint8_t get_ball (Ball *self)
{
    return (1 << (self->position->x / SCALER));
}

/** Gets the column of the ball.
 * @param self the ball. */
uint8_t get_ball_column (Ball *self)
{
    return self->position->y / SCALER;
}


/** Checks if the ball has hit the paddle. Occurs when the ball is in the
 * column ahead of the paddle
 * @return 1 if the ball hits the paddle otherwise 0. */
uint8_t check_ball_hit (Ball *self)
{
    return (get_ball_column (self) == MAX_Y) && (get_ball(self) & get_paddle ());
}


/** Updates the ledmat to display the ball's current postition
 * @param Address to the ball object */
void ball_update_display (Ball* self)
{
    ledmat_display_column (get_ball(self), get_ball_column(self));
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
