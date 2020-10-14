/** @file   communicate.h
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Send and receive.
*/

#ifndef COMMUNICATE_H
#define COMMUNICATE_H
#include "system.h"
#include "ball.h"


/** Checks if player is ready to start, indicated by a push of the navswitch
 * @return 1 if the navswitch has been pushed, otherwise 0 */
bool is_ready (void);


/** Checks if opponent is ready to start indicated by the reception
 * of a 'Ready' indicator
 * @return 1 if the opponent has indicated ready, otherwise 0 */
bool opponent_ready (void);


/** Sends the ready character to the opponent */
void send_ready (void);


/** Transfers the ball to the opponent's screen.
 * @param self the ball. */
void send_ball (Ball *self);

void receive_ball (Ball *ball, char message);
#endif
