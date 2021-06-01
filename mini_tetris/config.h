#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define DELAY_NANOSEC_PER_FRAME (100000000L)

enum {
    SCREEN_WIDTH = 7,
    SCREEN_HEIGHT = 10,

    DEFAULT_SCORE = 10,

    MAX_LEVEL = 5,
    LED_LEVEL = 4,
};

static const uint32_t FRAME_PER_LEVEL_UPS[1 + MAX_LEVEL + 1] = { 0, 300, 560, 780, 900, 1060, UINT32_MAX };
static const uint8_t DELAY_PER_LEVELS[1 + MAX_LEVEL + 1] = { 10 /* lower bound */,
                                                             10, 9, 8, 6, 4,
                                                             4 /* upper bound */ };

#endif /* CONFIG_H */