#include "system.h"
#include "ledmat.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "led.h"
#include "ir_uart.h"
#include "paddle.h"

#define PACER_RATE 500
#define COUNTDOWN_TIMER_RATE 500
#define MESSAGE_RATE 10

void tinygl_setup (void)
{
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
}

void game_init (void)
{
    system_init ();
    led_init ();  //led_set (LED1, 1); <- use to debug
    led_set (LED1, 0);
    tinygl_setup ();

    navswitch_init();
    ir_uart_init ();
}

uint8_t is_ready (void)
{
    navswitch_update ();
    return navswitch_push_event_p (NAVSWITCH_PUSH);
}


uint8_t opponent_is_ready (void)
{
    return ir_uart_read_ready_p () && ir_uart_getc () == 'R';
}

void wait_for (uint8_t (*func)(void))
{
    while (!(*func) ()) {
        pacer_wait();
        tinygl_update ();
    }
}

void startup (void)
{
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text ("READY UP");

    wait_for (is_ready);

    if (ir_uart_read_ready_p) { //Recieved opponent ready
        ir_uart_getc (); //cleared buffer
        ir_uart_putc('R'); // send back ready

    } else { // Haven't received opponent ready
        ir_uart_putc('R'); //send ready
        tinygl_clear();
        tinygl_text ("WAITING FOR OPPONENT");
        wait_for (opponent_is_ready); //wait for opponent ready
    }
}

void countdown (void)
{
    uint16_t pacer_counter = 0;
    char counter[] = {'3', '\0'};

    tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
    tinygl_clear ();

    while (counter[0] >= '0') {
        tinygl_update ();
        pacer_wait ();
        pacer_counter++;

        if (pacer_counter == COUNTDOWN_TIMER_RATE) {
            pacer_counter = 0;
            counter[0]--;
        }

        if (counter[0] == '0') {
            tinygl_text ("GO");
        } else {
            tinygl_text (counter);
        }
    }
}

int main (void)
{

    game_init ();
    pacer_init(PACER_RATE);
    startup ();
    countdown ();
    // Begin Game
    while (1)
    {
        pacer_wait();
        navswitch_update ();

        if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {

            paddle_shift_left ();
        }

        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            paddle_shift_right ();
        }
        ledmat_display_column (get_paddle(), 4);

    }
}
