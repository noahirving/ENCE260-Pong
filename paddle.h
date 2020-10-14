/** @file   paddle.h
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Paddle controller.
*/

#ifndef PADDLE_H
#define PADDLE_H

#include "system.h"

#define PADDLE_LENGTH 3


/** Changes the length of the paddle.
 * @param new length of paddle. */
void paddle_set_length (uint8_t length);


/** Sets the period of the paddle movement.
 * @param new period of the paddle. */
void paddle_set_period (uint16_t period);


/** Initializes the paddle.
 * @param new_length length of the paddle */
void paddle_init (uint8_t new_length);


/** Updates the paddles position when the navswitch is pushed. */
void paddle_update (void);


/** Gets the paddle as a single row bitmap.
 * @return 8 bit pattern representing the paddles location */
uint8_t get_paddle_pattern (void);


/** Updates the ledmat to display the paddle's current postition */
void paddle_update_display (void);

#endif
