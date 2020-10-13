#include "communicate.h"
#include "ir_uart.h"
#include "navswitch.h"

#define READY 'R'


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

void send_ready (void)
{
    ir_uart_putc(READY);
}
