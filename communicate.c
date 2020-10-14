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
void send_ball (Ball* self)
{
    char message = 0;
    // Inverts x position.*/
    uint8_t position_x = MAX_X - ball_get_position (self).x;

    // Inverts direction for mirrored receiver
    invert_x_direction (self);

    // Encodes ball into char.
    message |= SET_MASK (position_x, 0, 0b111);
    message |= SET_MASK (self->direction_vector, 3, 0b111);
    message |= SET_MASK (self->speed_increased, 6, 0b1);

    if (self->speed_increased) {
        self->speed_increased = false;
    }

    ir_uart_putc (message); // Transfer ball position and vector as single character
}

/** Receives the ball from the opponent's screen.
 * @param ball the ball to set
 * @param message encoded ball. */
void receive_ball (Ball *ball, char message)
{
    uint8_t position_x = GET_MASK (message, 0, 0b111);
    uint8_t direction_vector = GET_MASK (message, 3, 0b111);
    uint8_t speed_increased = GET_MASK (message, 6, 0b1);

    ball->position->x = position_x * SCALER;
    ball->position->y = MIN_Y * SCALER;

    ball->y_direction = 1;
    ball->direction_vector = direction_vector;

    if (speed_increased) {
        ball_increase_speed (ball);
        ball->speed_increased = false;
    }
}


/** Returns if the message is an encoded ball.
 * @param message possible encoded ball. */
bool is_ball (char message)
{
    // Message is ball if 8th bit is '0'
    return message & BIT(8);
}

Message_type get_message (char* message)
{
    if (ir_uart_read_ready_p ()) {
        *message = ir_uart_getc ();
        if (is_ball (*message)) {
            return MESSAGE_BALL;
        } else {
            return MESSAGE_SCORE;
        }
    } else {
        return MESSAGE_EMPTY;
    }
}
