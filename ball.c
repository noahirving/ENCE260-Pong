#include "paddle.h"
#include "system.h"
#include "ball.h"
#include "pio.h"
#include "ir_uart.h"


#define SCALER 10
#define MIN_X (0 * SCALER)
#define MAX_X (6 * SCALER)
#define MIN_Y (0 * SCALER)
#define MAX_Y (4 * SCALER)

Ball new_ball(Vector *direction, Vector *position, uint8_t speed)
{
    position->x = position->x * SCALER;
    position->y = position->y * SCALER;
    Ball ball = {direction, position, speed};
    return ball;
}



const Vector directions[] = {
    {0, 1},
    {1, 1},
    {-1, 1},
    {1, 2},
    {-1, 2}
};

/** Initializes the ball to be at the centre of the board */
void ball_init (void)
{
    //ball_col = 0;
    //ball_row = 3;
}

/** Transfers the ball to the opponent's screen */
void transfer_ball (void)
{
    ir_uart_putc (0); // Transfer ball position and vector as single character
}

/** Receives the ball from the opponent's screen */
void receive_ball (void)
{

}

bool is_ball (char message)
{
    return ~(message >> 7);
}

bool can_collide (Ball *self)
{
    return get_ball_column (self) == MAX_Y;
}

bool is_colliding (Ball *self,  uint8_t paddle_bitmap)
{
    return get_ball (self) & paddle_bitmap;
}



/** Updates the balls position based on its direction */
void ball_update_position (Ball *self)
{
    self->position->x += self->direction->x * self->speed;
    self->position->y += self->direction->y * self->speed;

    if (self->position->x <= MIN_X) {
        self->position->x = MIN_X;
        self->direction->x = -self->direction->x;
    } else if (self->position->x >= MAX_X) {
        self->position->x = MAX_X;
        self->direction->x = -self->direction->x;
    }

    if (self->position->y <= MIN_Y) {
        self->position->y = MIN_Y;
        self->direction->y = -self->direction->y;
    } else if (self->position->y >= MAX_Y) {
        self->position->y = MAX_Y;
        self->direction->y = -self->direction->y;
    }
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
