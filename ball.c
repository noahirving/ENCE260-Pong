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

static const Vector directions[] = {
    {-2, 1},
    {-1, 1},
    {-1, 2},
    {0, 1},
    {1, 2},
    {1, 1},
    {2, 1}
};

Ball new_ball(uint8_t direction_vector, Vector *position, uint8_t speed)
{
    position->x = position->x * SCALER;
    position->y = position->y * SCALER;
    Ball ball = {direction_vector, 1, position, speed};
    return ball;
}

static void invert_x_direction (Ball *self)
{
    self->direction_vector = -(self->direction_vector - NUM_DIRECTIONS / 2) + NUM_DIRECTIONS / 2;
}

bool ball_is_transferable (Ball *self)
{
    return self->position->y  < MIN_Y && self->y_direction < 0;
}

/** Transfers the ball to the opponent's screen */
void transfer_ball (Ball *self)
{
    uint8_t position_x = MIN_X / SCALER + MAX_X / SCALER - self->position->x / SCALER;

    invert_x_direction (self);
    uint8_t direction_vector = self->direction_vector;

    char message = (direction_vector << 3) | position_x;
    ir_uart_putc (message); // Transfer ball position and vector as single character
}

/** Receives the ball from the opponent's screen */
void receive_ball (Ball *ball, char message)
{
    uint8_t position_x = message & 0b111;
    uint8_t direction_vector = (message >> 3) & 0b111;

    ball->position->x = position_x * SCALER;
    ball->position->y = MIN_Y * SCALER;
    ball->y_direction = 1;
    ball->direction_vector = direction_vector;
    ball->speed = 10;
}

bool is_ball (char message)
{
    return ~(message >> 7);
}

bool can_collide (Ball *self)
{
    return self->position->y >= MAX_Y;
}

bool is_colliding (Ball *self,  uint8_t paddle_bitmap)
{
    return get_ball (self) & paddle_bitmap;
}

Vector ball_get_direction (Ball *self)
{
    Vector direction;
    direction.x = directions[self->direction_vector].x;
    direction.y = directions[self->direction_vector].y * self->y_direction;
    return direction;
}
void ball_bounce_paddle (Ball *self)
{
    Vector direction = ball_get_direction (self);
    self->position->x -= direction.x * self->speed;
    self->position->y -= direction.y * self->speed;

    self->y_direction = -self->y_direction;

    ball_update_position (self);
}

void ball_bounce_wall (Ball * self)
{
    if (self->position->x <= MIN_X) {
        self->position->x = MIN_X;
        invert_x_direction (self);

    } else if (self->position->x >= MAX_X) {
        self->position->x = MAX_X;
        invert_x_direction (self);
    }
}

/** Updates the balls position based on its direction */
void ball_update_position (Ball *self)
{
    Vector direction = ball_get_direction (self);
    self->position->x += direction.x * self->speed;
    self->position->y += direction.y * self->speed;

    // TODO: Remove for transfering
    /*
    if (self->position->y <= MIN_Y) {
        self->position->y = MIN_Y;
        self->y_direction = -self->y_direction;
    }*/
}

/** Gets the position of the ball as a bit pattern */
uint8_t get_ball (Ball *self)
{
    return (1 << (self->position->x / SCALER));
}

/** Gets the column of the ball */
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
    uint8_t num_flashes = 0;
    while (num_flashes < 4) {
        pacer_wait ();
        paddle_update_display ();
        pacer_wait ();

        if (pacer_counter == 67) {
            ball_update_display (self);
            pacer_counter++;
        }

        if (pacer_counter == 125) {
            ledmat_display_column (0, get_ball_column(self));
            num_flashes++;
            pacer_counter = 0;
        }

        pacer_counter++;
    }
}
