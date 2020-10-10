#include "paddle.h"
#include "system.h"

static uint8_t ball_col;
static uint8_t ball_row;

/** Checks if the ball has hit the paddle. Occurs when the ball is in the
 * column ahead of the paddle
 * @return 1 if the ball hits the paddle otherwise 0 */
bool check_ball_hit (void)
{
    uint8_t paddle = get_paddle;
    return (ball_row & (paddle & (1 << ball_row)));
}

/** Initializes the ball to be at the centre of the board */
void ball_init (void)
{
    ball_col = 0;
    ball_row = 3;
}

/** Transfers the ball to the opponent's screen */
void transfer_ball (void)
{

}

/** Receives the ball from the opponent's screen */
void receive_ball (void)
{

}

/** Updates the balls position based on its direction */
void ball_update (void)
{
    // change position based on vector
}

/** Sets the direction of the ball */
void ball_set_vector (int vector)
{
    // set vector when passing to opponent or hitting wall
}

/** Gets the position of the ball as a bit pattern */
uint8_t get_ball (void)
{
    return (1 << ball_row);
}

/** Gets the column of the ball */
uint8_t get_ball_column (void)
{
    return ball_col;
}
