#include "system.h"

#define PADDLE_LENGTH 2
#define MIN_POSITION 0
#define MAX_POSITION 5
static uint8_t paddle_position = 0;

static uint8_t get_paddle (void)
{
    return (1 << paddle_position) | (1 << (paddle_position + 1));

}
