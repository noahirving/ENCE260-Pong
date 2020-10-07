#include "system.h"
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

void display_countdown (char character)
{
    char buffer[3];
    buffer[0] = '0';
    buffer[1] = character;
    buffer[2] = '\0';
    tinygl_text (buffer);
}

void tinygl_setup ()
{
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
}

void init ()
{
    system_init ();
    led_init ();  //led_set (LED1, 1); <- use to debug
    led_set (LED1, 0);
    tinygl_setup ();

    navswitch_init();
    ir_uart_init ();
}

uint8_t is_ready ()
{
    navswitch_update ();
    return navswitch_push_event_p (NAVSWITCH_PUSH);
}


uint8_t opponent_is_ready ()
{
    return ir_uart_read_ready_p () && ir_uart_getc () == 'R';
}

void wait_for (uint8_t (*func)()) {
    while (!(*func) ()) {
        pacer_wait();
        tinygl_update ();
    }
}

void startup ()
{
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text ("READY UP");

    wait_for (is_ready);

    ir_uart_putc('R');

    tinygl_clear();
    tinygl_text ("WAITING FOR OPPONENT");
    wait_for (opponent_is_ready);
}

int main (void)
{
    init ();
    pacer_init(PACER_RATE);
    startup ();

    tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
    tinygl_clear ();

    int start_countdown = '3';
    uint16_t pacer_counter = 0;
    while (start_countdown >= '0') {

        if (start_countdown == '0') {
            tinygl_text ("GO");
        } else {
            display_countdown (start_countdown);
        }

        tinygl_update ();
        pacer_wait ();
        pacer_counter++;

        if (pacer_counter == COUNTDOWN_TIMER_RATE) {
            pacer_counter = 0;
            start_countdown--;
        }
    }

    paddle_init ();
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

    }

}
