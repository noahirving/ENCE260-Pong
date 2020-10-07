#include "system.h"
#include "pio.h"

#define PADDLE_LENGTH 3
#define BOARD_WIDTH 7
#define MIN_POSITION 0
#define MAX_POSITION (BOARD_WIDTH - PADDLE_LENGTH)

/** Define PIO pins driving LED matrix rows.  */
static const pio_t rows[] =
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};


/** Define PIO pins driving LED matrix columns.  */
static const pio_t cols[] =
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};

static uint8_t paddle;

static uint8_t paddle_position = 2;

uint8_t get_paddle (void)
{
    paddle = 0;
    for (uint8_t i = 0; i < PADDLE_LENGTH; i++) {
        paddle |= (1 << (paddle_position + i));
    }

    return paddle;
}

void paddle_update ()
{
    for (uint8_t current_row = 0; current_row < LEDMAT_ROWS_NUM; current_row++) {
        if ((paddle >> current_row) & 1) {
            pio_output_low(rows[current_row]);
        } else {
            pio_output_high(rows[current_row]);
        }
    }
}

void paddle_init ()
{
    for (uint8_t col_num = 0; col_num < LEDMAT_COLS_NUM; col_num++) {
        if (col_num == 4) {
            pio_output_low(cols[col_num]);
        } else {
            pio_output_high(cols[col_num]);
        }
    }
    paddle = get_paddle ();
    paddle_update ();
}

void paddle_shift_left () {
    if (paddle_position < MAX_POSITION) {
        paddle_position++;
        paddle = get_paddle ();
        paddle_update ();
    }
}

void paddle_shift_right () {
    if (paddle_position > MIN_POSITION) {
        paddle_position--;
        paddle = get_paddle ();
       paddle_update ();
   }
}
