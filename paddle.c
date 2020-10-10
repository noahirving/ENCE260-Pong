/** @file   paddle.c
    @author
    @date
    @brief  Paddle controller.
*/
#include "pio.h"
#include "navswitch.h"
#include "paddle.h"

#define MIN_POSITION 0
#define BOARD_WIDTH LEDMAT_ROWS_NUM
#define LEFT NAVSWITCH_SOUTH
#define RIGHT NAVSWITCH_NORTH


static uint8_t paddle_length = 3;
static uint8_t paddle_position = 0;
static uint16_t paddle_counter = 0;
static uint16_t move_period = 0;
static uint8_t max_position = 0;


/** Shifts the paddle position to the left if it does not move
    beyond the max position. */
static void paddle_shift_left (void)
{
    if (paddle_position < max_position) {
        paddle_position++;
    }
}


/** Shifts the paddle position to the right if it does not move
    beyond the min position. */
static void paddle_shift_right (void)
{
    if (paddle_position > MIN_POSITION) {
        paddle_position--;
   }
}


/** Initializes the paddle.
 * @param new_length length of the paddle
 * @param new_period paddle update period */
void paddle_init (uint8_t new_length, uint16_t new_period)
{
    paddle_set_length (new_length);
    paddle_set_period (new_period);

    /* Sets the paddle position to the center. */
    paddle_position = (BOARD_WIDTH / 2) - (paddle_length / 2) + MIN_POSITION;
}


/** Updates the paddles position when the navswitch is pushed. */
void paddle_update (void)
{
    /* If the counter has reached the period.
     * Else the counter is incremented. */
    if (paddle_counter >= move_period) {
        /* If the navwitch is pushed down left the paddle is
           moved left and the counter is reset.
         * Else if the paddle is pushed down right the paddle is
           moved right and the counter is reset */
        if (navswitch_down_p (LEFT)) {
            paddle_shift_left ();
            paddle_counter = 0;
        } else if (navswitch_down_p (RIGHT)) {
            paddle_shift_right ();
            paddle_counter = 0;
        }
    } else {
        paddle_counter++;
    }
}


/** Gets the paddle as a single row bitmap. */
uint8_t get_paddle (void)
{
    uint8_t paddle = 0;
    for (uint8_t i = 0; i < paddle_length; i++) {
        paddle |= (1 << (paddle_position + i));
    }
    return paddle;
}


/** Changes the length of the paddle.
 * @param new length of paddle. */
void paddle_set_length (uint8_t new_length)
{
    /* Sets the new max position. */
    max_position = MIN_POSITION + BOARD_WIDTH - new_length;

    /* If the current position of the paddle is more than the new
       max position, sets the position back to the max position. */
    if (max_position > paddle_position) {
        paddle_position = max_position;
    }

    /* Sets the new max length. */
    paddle_length = new_length;
}


/** Sets the period of the paddle movement.
 * @param new period of the paddle. */
void paddle_set_period (uint16_t period)
{
    move_period = period;
}


