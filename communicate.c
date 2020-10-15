/** @file   communicate.c
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Send and receive.
*/

#include "ir_uart.h"
#include "navswitch.h"

#include "communicate.h"
#include "ball.h"

#define CONTROL_PATTERN BIT(7)

#define IS_BALL(MESSAGE) ((~MESSAGE) & CONTROL_PATTERN)
#define IS_SCORE(MESSAGE) (MESSAGE & CONTROL_PATTERN)

#define READY 'R'
#define LOST CONTROL_PATTERN

#define POSITION_X_POSITION 0
#define POSITION_X_MASK 0b111


#define DIRECTION_VECTOR_POSITION 3
#define DIRECTION_VECTOR_MASK 0b111


#define SPEED_INCREASED_POSITION 6
#define SPEED_INCREASED_MASK 0b1

#define GET_MASK(NUM,POS,MASK) ((NUM >> POS) & MASK)
#define SET_MASK(NUM,POS,MASK) ((NUM & MASK) << POS)



void communicate_init (void)
{
    ir_uart_init ();
}

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



/** Sends the READY character to the opponent. */
void send_ready (void)
{
    ir_uart_putc(READY);
}



/** Sends the LOST character to the opponent. */
void send_lost (void)
{
    ir_uart_putc(LOST);
}



/** Transfers the ball to the opponent's screen.
 * @param ball Address of the ball. */
void send_ball (Ball* ball)
{
    char message = 0;
    // Inverts x position.*/
    uint8_t position_x = MAX_X - ball_get_position (ball).x;

    // Inverts direction for mirrored receiver
    invert_x_direction (ball);

    // Encodes ball into char.
    message |= SET_MASK (position_x,
                         POSITION_X_POSITION, POSITION_X_MASK);

    message |= SET_MASK (ball->direction_vector,
                         DIRECTION_VECTOR_POSITION, DIRECTION_VECTOR_MASK);

    message |= SET_MASK (ball->speed_increased,
                         SPEED_INCREASED_POSITION, SPEED_INCREASED_MASK);

    if (ball->speed_increased) {
        ball->speed_increased = false;
    }

    ir_uart_putc (message); // Transfer ball position and vector as single character

    ball->active = false;
}



/** Receives the ball from the opponent's screen.
 * @param ball the ball to set
 * @param message encoded ball. */
void receive_ball (Ball *ball, char message)
{
    uint8_t position_x = GET_MASK (message,
                         POSITION_X_POSITION, POSITION_X_MASK);

    uint8_t direction_vector = GET_MASK (message,
                         DIRECTION_VECTOR_POSITION, DIRECTION_VECTOR_MASK);

    uint8_t speed_increased = GET_MASK (message,
                         SPEED_INCREASED_POSITION, SPEED_INCREASED_MASK);

    ball->position->x = position_x * SCALER;
    ball->position->y = MIN_Y * SCALER;

    ball->y_direction = 1;
    ball->direction_vector = direction_vector;

    if (speed_increased) {
        ball->speed++;
        ball->speed_increased = false;
        ball->hit_counter = 0;
    }

    ball->active = true;
}



/** Gets the type of message passed through infra-red
 * @param message The message being sent
 * @return Message_type enum representing either a score, ball, or indicates if empty */
Message_type get_message (char* message)
{
    if (ir_uart_read_ready_p ()) {
        *message = ir_uart_getc ();
        if (IS_BALL (*message)) {
            return MESSAGE_BALL;
        } else if (IS_SCORE (*message)) {
            return MESSAGE_SCORE;
        }
    }
    return MESSAGE_EMPTY;
}
