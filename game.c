/** @file   game.c
    @author Noah Irving (nji29), David Frost (djf99)
    @date   15/10/2020
    @brief  Main game program.
*/

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
#include "communicate.h"

#define PACER_RATE 500
#define COUNTDOWN_TIMER_RATE 500
#define MESSAGE_RATE 20

// Global variable defining who will start each round
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
    scoring_init();
    tinygl_setup ();
    navswitch_init ();
    ir_uart_init ();
    pacer_init (PACER_RATE);
}


/** Waits until the passed function returns true
 * @param *func function passed in to be evaluated for true or false */
void wait_for (bool (*func)(void))
{
    while (!(*func) ()) {
        pacer_wait ();
        tinygl_update ();
    }
}


/** Connects to the opponent by sending ready
 *  to them and waiting for response */
void connect (void)
{
    starting_player = false;
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text ("  READY UP");

    wait_for (is_ready);

    if (!opponent_ready ()) {
        tinygl_clear ();
        tinygl_text ("  WAITING FOR OPPONENT");
        send_ready ();
        wait_for (opponent_ready);
        starting_player = true;
    } else {
        send_ready ();
    }
}


/** Displays countdown before each round starts*/
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
    uint8_t ball_hit_counter = 0;

    // Only display ball for the player who is starting
    if (starting_player) {
        ball_on_screen = true;
    }

    countdown ();
    paddle_init (PADDLE_LENGTH);

    uint16_t ball_counter = 0;
    Vector position = DEFAULT_BALL_POSITION;
    Ball my_ball = new_ball (DEFAULT_BALL_DIRECTION, &position, BALL_MIN_SPEED);

    // Begin Round
    while (round_running) {
        pacer_wait ();
        navswitch_update ();
        paddle_update ();
        paddle_update_display ();

        pacer_wait (); // Second pacer wait allows display to switch between dislaying the paddle and the ball

        // Only performs update for the player who has the ball on their screen
        if (ball_on_screen) {
            if (ball_counter >= BALL_UPDATE_PERIOD) {
                ball_counter = 0;
                ball_update_position (&my_ball);

                if (can_collide (&my_ball)) { //Ball is in the row with the paddle
                    if (is_colliding (&my_ball, get_paddle ())) { //Ball has hit the paddle and bounced off
                        ball_bounce_paddle (&my_ball);
                        ball_hit_counter++;
                    }
                    else { //Ball has missed the paddle so the round is lost
                        flash_ball (&my_ball);
                        round_running = false;
                        ball_on_screen = false;
                        starting_player = true;
                        lost_round ();
                    }
                }

                ball_bounce_wall (&my_ball);

                if (ball_hit_counter >= BALL_SPEED_INC_PERIOD) {
                    ball_increase_speed (&my_ball);
                    ball_hit_counter = 0;
                }

                if (ball_is_transferable (&my_ball)) { // Ball is on the display boundary
                    send_ball (&my_ball);
                    ball_on_screen = false;
                }

            }
            if (ball_on_screen) {
                ball_update_display (&my_ball);
            }
        }

        if (ir_uart_read_ready_p ()) {

            char message = ir_uart_getc ();

            if (is_score (message)) {
                // Opponent lost the round
                starting_player = false;
                opponent_lost_round ();
                round_running = false;

            } else if (is_ball (message)) {
                // Ball has reached edge of opponent's screen and transferred it over
                if (message & BIT(6)) { //Ball speed increased on opponent's screen
                    ball_hit_counter = 0;
                }
                receive_ball (&my_ball, message);
                ball_on_screen = true;
            }
        }

        ball_counter ++;
    }
}


int main (void)
{
    while (1) {
        game_init ();
        connect ();

        // Begin Game
        while (!game_finished ()) {
            play_round ();
            display_score ();
        }

        end_game ();
    }
}
