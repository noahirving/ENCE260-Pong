#include "system.h"
#include "ledmat.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "led.h"
#include "ir_uart.h"
#include "paddle.h"
#include "ball.h"

#define PACER_RATE 500
#define COUNTDOWN_TIMER_RATE 500
#define MESSAGE_RATE 10
#define PADDLE_PERIOD 40

bool starting_player = false;

/** Set up and initialise text display for ledmat*/
void tinygl_setup (void)
{
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
}

/** Initialize system and drivers*/
void game_init (void)
{
    system_init ();
    //led_init ();  //led_set (LED1, 1); <- use to debug
    //led_set (LED1, 0);
    tinygl_setup ();

    navswitch_init();
    ir_uart_init ();
}

/** Checks if player is ready to start*/
uint8_t is_ready (void)
{
    navswitch_update ();
    return navswitch_push_event_p (NAVSWITCH_PUSH);
}

/** Checks if opponent is ready to start*/
uint8_t opponent_is_ready (void)
{
    return ir_uart_read_ready_p () && ir_uart_getc () == 'R';
}

/** Passes the is_ready functions and waits until they are true
 * @param *func function passed in to be evaluated for true or false */
void wait_for (uint8_t (*func)(void))
{
    while (!(*func) ()) {
        pacer_wait();
        tinygl_update ();
    }
}

/** Startup sequence of the game, waits for both players to ready up before starting*/
void startup (void)
{
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text ("READY UP");

    wait_for (is_ready);

    if (ir_uart_read_ready_p ()) {
        // Opponent has readied up first, so send ready back so they can stop waiting
        ir_uart_getc ();
        ir_uart_putc('R');

    } else {
        // First person to ready up, send ready to opponent and wait for response
        // First person to ready up will also start the first round
        ir_uart_putc('R');
        tinygl_clear();
        tinygl_text ("WAITING FOR OPPONENT");
        wait_for (opponent_is_ready);
        starting_player = true;
    }
}

/** Displays countdown after players are ready before the game starts*/
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

        // Display "GO" when the counter reaches 0
        if (counter[0] == '0') {
            tinygl_text ("GO");
        } else {
            tinygl_text (counter);
        }
    }
}

int main (void)
{
    bool ball_on_screen = false;

    game_init ();
    pacer_init(PACER_RATE);
    startup ();

    // Only display ball for the player who is starting
    if (starting_player) {
        ball_init ();
        ball_on_screen = true;
    }

    countdown ();
    paddle_set_period (PADDLE_PERIOD);

    // Begin Game
    while (1)
    {
        pacer_wait();
        navswitch_update ();

        // Only performs update for the player who has the ball on their screen
        if (ball_on_screen) {
            ball_update_position ();
            ledmat_display_column (get_ball(), get_ball_column()); // display ball
        }

        pacer_wait ();
        paddle_update ();
        ledmat_display_column (get_paddle(), 4); // display paddle


    }
}
