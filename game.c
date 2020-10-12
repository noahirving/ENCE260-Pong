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
#include "scoring.h"

#define PACER_RATE 500
#define COUNTDOWN_TIMER_RATE 500
#define MESSAGE_RATE 20
#define PADDLE_LENGTH 3
#define DEFAULT_BALL_SPEED 5
#define DEFAULT_BALL_DIRECTION 3
#define DEFAULT_BALL_POSITION {3, 0}
#define BALL_UPDATE_PERIOD 40
#define READY 'R'


bool starting_player = false;


/** Set up and initialise text display for ledmat*/
void tinygl_setup (void)
{
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
}


/** Initialize system and drivers */
void game_init (void)
{
    system_init ();
    tinygl_setup ();
    navswitch_init();
    ir_uart_init ();
    pacer_init(PACER_RATE);
}


/** Checks if player is ready to start, indicated by a push of the navswitch
 * @return 1 if the navswitch has been pushed, otherwise 0 */
uint8_t is_ready (void)
{
    navswitch_update ();
    return navswitch_push_event_p (NAVSWITCH_PUSH);
}


/** Checks if opponent is ready to start indicated by the reception
 * of a 'Ready' indicator
 * @return 1 if the opponent has indicated ready, otherwise 0 */
uint8_t opponent_is_ready (void)
{
    return ir_uart_read_ready_p () && ir_uart_getc () == READY;
}


/** Waits until the passed function returns true
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
    tinygl_text ("  READY UP");

    wait_for (is_ready);

    if (ir_uart_read_ready_p ()) {
        // Opponent has readied up first, so send ready back so they can stop waiting
        ir_uart_getc ();
        ir_uart_putc(READY);

    } else {
        // First person to ready up, send ready to opponent and wait for response
        // Whis player will also start the first round
        ir_uart_putc(READY);
        tinygl_clear();
        tinygl_text ("  WAITING FOR OPPONENT");
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
            tinygl_point_t pos = {0, 4}; // Position of character is set to the middle of the rotated ledmat
            tinygl_draw_message (counter, pos, 1);
        }
    }
}


/** Starts a single round. Plays until one of the players miss the ball */
void play_round (void)
{
    bool round_running = true;
    bool ball_on_screen = false;

    /* Only display ball for the player who is starting */
    if (starting_player) {
        ball_on_screen = true;
    }

    countdown ();
    paddle_init (PADDLE_LENGTH);

    uint16_t ball_tick_counter = 0;
    Vector position = DEFAULT_BALL_POSITION;
    Ball my_ball = new_ball (DEFAULT_BALL_DIRECTION, &position, DEFAULT_BALL_SPEED);

    // Begin Game
    while (round_running) {
        pacer_wait ();
        navswitch_update ();
        paddle_update ();
        paddle_update_display(); // display paddle

        pacer_wait();

        // Only performs update for the player who has the ball on their screen
        if (ball_on_screen) {
            if (ball_tick_counter >= BALL_UPDATE_PERIOD) {
                ball_tick_counter = 0;

                ball_update_position (&my_ball);
                if (can_collide (&my_ball)) {
                    if (is_colliding (&my_ball, get_paddle ())) {
                        ball_bounce_paddle (&my_ball);
                    }
                    else {
                        flash_ball (&my_ball);
                        round_running = false;
                        ball_on_screen = false;
                        starting_player = true;

                        lost_round ();
                    }
                }
                ball_bounce_wall (&my_ball);


                if (ball_is_transferable (&my_ball)) {
                    transfer_ball (&my_ball);
                    ball_on_screen = false;
                }

            }
            if (ball_on_screen) {
                ball_update_display (&my_ball);
            }
        }

        if (ir_uart_read_ready_p ()) {

            char message = ir_uart_getc ();

            if (is_score(message)) {
                /* Opponent lost the round */
                starting_player = false;
                opponent_lost_round ();
                round_running = false;

            } else {
                /* Ball has reached edge of opponents screen and transfered it over */
                receive_ball (&my_ball, message);
                ball_on_screen = true;
            }
        }

        ball_tick_counter++;
    }
}


int main (void)
{
    game_init ();
    startup ();

    // Begin Game
    while (!game_finished ()) {
        play_round ();
        display_score ();
    }

    end_game ();
}
