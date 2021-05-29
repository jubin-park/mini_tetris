#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

enum {
    BLOCK_WIDTH = 3,
    BLOCK_HEIGHT = 3,
    BLOCK_COUNT = 4
};

typedef enum {
    ANGLE_0,
    ANGLE_90,
    ANGLE_180,
    ANGLE_270,
    ANGLE_SIZE
} angle_t;

typedef struct {
    int8_t x;
    int8_t y;
    angle_t angle;
    const uint8_t* tile_of_zero_angle;
} block_t;

const uint8_t BLOCK_TILES[BLOCK_COUNT * BLOCK_HEIGHT * ANGLE_SIZE][BLOCK_WIDTH] =
{
/* block 0 */
    {0, 1, 0},
    {1, 1, 1},
    {0, 0, 0},

    {0, 1, 0},
    {0, 1, 1},
    {0, 1, 0},

    {0, 0, 0},
    {1, 1, 1},
    {0, 1, 0},

    {0, 1, 0},
    {1, 1, 0},
    {0, 1, 0},

/* block 1 */
    {1, 0, 0},
    {1, 1, 1},
    {0, 0, 0},

    {0, 1, 1},
    {0, 1, 0},
    {0, 1, 0},

    {0, 0, 0},
    {1, 1, 1},
    {0, 0, 1},

    {0, 1, 0},
    {0, 1, 0},
    {1, 1, 0},

/* block 2 */
    {1, 1, 0},
    {0, 1, 1},
    {0, 0, 0},

    {0, 0, 1},
    {0, 1, 1},
    {0, 1, 0},

    {0, 0, 0},
    {1, 1, 0},
    {0, 1, 1},

    {0, 1, 0},
    {1, 1, 0},
    {1, 0, 0},

/* block 3 */
    {0, 0, 0},
    {1, 1, 1},
    {0, 0, 0},

    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},

    {0, 0, 0},
    {1, 1, 1},
    {0, 0, 0},

    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
};

#endif /* BLOCK_H */