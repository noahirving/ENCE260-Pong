
#define PADDLE_LENGTH 2
#define MIN_POSITION 0
#define MAX_POSITION 5
static uint8_t paddle_position = 0;

static uint8_t get_paddle (void)
{
    uint8_t paddle = 0;
    for (uint8_t i = 0; i < PADDLE_LENGTH; i++) {
        paddle |= (1 << (paddle_position + i));
    }
    return (1 << paddle_position) | (1 << (paddle_position + 1);

}
