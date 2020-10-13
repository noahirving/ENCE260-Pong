/** @file   communicate.c
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Send and receive.
*/

#include "communicate.h"
#include "ir_uart.h"
#include "navswitch.h"
#include "ball.h"

#define READY 'R'
#define GET_MASK(NUM,POS,MASK) ((NUM >> POS) & MASK)
#define SET_MASK(NUM,POS,MASK) ((NUM & MASK) << POS)
#define ENCODE_BALL(SPEED_INCREASED,DIRECTION_VECTOR,POSITION_X) \
    SET_MASK (SPEED_INCREASED, 6, 0b1) | \
    SET_MASK (DIRECTION_VECTOR, 3, 0b111) | \
    SET_MASK (POSITION_X, 0, 0b111)


/** Checks if player is ready to start, indicated by a push of the navswitch
 * @return 1 if the navswitch has been pushed, otherwise 0 */
bool is_ready (void)
{
    navswitch_update ();
    return navswitch_push_event_p (NAVSWITCH_PUSH);
}


/** Checks if opponent is ready to start indicated by the reception
 * of a 'Ready' indicator
 * @return 1 if the opponent has indicated ready, otherwise 0 */
bool opponent_ready (void)
{
    return ir_uart_read_ready_p () && ir_uart_getc () == READY;
}


/** Sends the ready character to the opponent */
void send_ready (void)
{
    ir_uart_putc(READY);
}


/** Transfers the ball to the opponent's screen.
 * @param self the ball. */
void send_ball (Ball *self)
{
    char message;
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

    // Encodes ball into char.
    message = ENCODE_BALL(self->speed_increased,self->direction_vector,position_x);
    if (self->speed_increased) {
        self->speed_increased = false;
    }

    ir_uart_putc (message); // Transfer ball position and vector as single character
}
