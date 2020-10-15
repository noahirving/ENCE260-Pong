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

#include "paddle.h"
#include "ball.h"
#include "scoring.h"
#include "communicate.h"

#define PACER_RATE 500
#define COUNTDOWN_TIMER_RATE 500
#define MESSAGE_RATE 20
#define COUNTDOWN_POSITION {0, 4}
#define COUNTDOWN_START '3'
#define COUNTDOWN_END '0'


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
    communicate_init ();
    ledmat_init ();
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
    char counter[] = {COUNTDOWN_START, '\0'};

    tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
    tinygl_clear ();

    while (counter[0] >= COUNTDOWN_END) {
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
            tinygl_point_t position = COUNTDOWN_POSITION;
            tinygl_draw_message (counter, position, 1);
        }
    }
}



/** Starts a single round. Plays until one of the players miss the ball */
void play_round (void)
{
    bool round_running = true;
    uint16_t ball_counter = 0;
    Vector position = DEFAULT_BALL_POSITION;
    Ball ball = new_ball (DEFAULT_BALL_DIRECTION, &position, BALL_MIN_SPEED);

    countdown ();
    paddle_init (PADDLE_LENGTH);

    if (starting_player) {
        ball.active = true;
    }

    while (round_running) {
        pacer_wait ();
        paddle_update ();
        paddle_update_display ();

        pacer_wait (); // Second pacer wait allows display to switch between dislaying the paddle and the ball

        // Only performs update for the player who has the ball on their screen

        if (ball_counter >= BALL_UPDATE_PERIOD) {
            ball_counter = 0;
            Ball_state state = ball_update (&ball);
            if (state == BALL_MISSED) {
                flash_ball (&ball);
                round_running = false;
                starting_player = true;
                lost_round ();
            } else if (state == BALL_SEND) {
                send_ball (&ball);
            }
        }

        if (ball.active) {
            ball_update_display (&ball);
        } else {
            char message = 0;
            Message_type type = get_message (&message);
            if (type == MESSAGE_BALL) {
                // Ball has reached edge of opponent's screen and transferred it over
                receive_ball (&ball, message);
            } else if (type == MESSAGE_SCORE) {
                starting_player = false;
                opponent_lost_round ();
                round_running = false;
            }
        }

        ball_counter++;
    }
}



/** Game loop. */
int main (void)
{
    while (1) {
        game_init ();
        connect ();

        while (!game_finished ()) {
            play_round ();
            display_score ();
        }

        end_game ();
        wait_for (is_ready);
    }
}
