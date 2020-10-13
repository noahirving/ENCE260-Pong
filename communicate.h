#ifndef COMMUNICATE_H
#define COMMUNICATE_H
#include "system.h"
/** Checks if player is ready to start, indicated by a push of the navswitch
 * @return 1 if the navswitch has been pushed, otherwise 0 */
bool is_ready (void);

/** Checks if opponent is ready to start indicated by the reception
 * of a 'Ready' indicator
 * @return 1 if the opponent has indicated ready, otherwise 0 */
bool opponent_ready (void);

void send_ready (void);

void clear_buffer (void);


#endif
