/** @file   communicate.h
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Send and receive.
*/

#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include "system.h"

#include "ball.h"

typedef enum {
    MESSAGE_EMPTY,
    MESSAGE_BALL,
    MESSAGE_SCORE
} Message_type;


void communicate_init (void);


/** Checks if player is ready to start, indicated by a push of the navswitch
 * @return 1 if the navswitch has been pushed, otherwise 0 */
bool is_ready (void);


/** Checks if opponent is ready to start indicated by the reception
 * of a 'Ready' indicator
 * @return 1 if the opponent has indicated ready, otherwise 0 */
bool opponent_ready (void);


/** Sends the READY character to the opponent. */
void send_ready (void);

/** Sends the LOST character to the opponent. */
void send_lost (void);

/** Transfers the ball to the opponent's screen.
 * @param self Address of the ball. */
void send_ball (Ball *self);


/** Receives the ball from the opponent's screen.
 * @param ball the ball to set
 * @param message encoded ball. */
void receive_ball (Ball *ball, char message);


/** Gets the type of message passed through infra-red
 * @param message The message being sent
 * @return Message_type enum representing either a score, ball, or indicates if empty */
Message_type get_message (char* message);

#endif
