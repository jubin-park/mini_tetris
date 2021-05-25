#ifndef BLOCKS_H
#define BLCOKS_H

#include <stdint.h>

typedef enum {
    ANGLE_0,
    ANGLE_90,
    ANGLE_180,
    ANGLE_270,
    ANGLE_SIZE
} angle_t;

typedef struct {
    unsigned char x;
    unsigned char y;
    angle_t angle;
} block_status_t;

static const uint8_t block1[4][3][3] =
{
    {   
        {1, 1, 1},
        {0, 1, 0},
        {0, 0, 0}
    },
    {
        {0, 0, 1},
        {0, 1, 1},
        {0, 0, 1}
    },
    {
        {0, 0, 0},
        {0, 1, 0},
        {1, 1, 1}
    },
    {
        {1, 0, 0},
        {1, 1, 0},
        {1, 0, 0}
    }
};

static const uint8_t block2[4][3][3] =
{
    {
        {1, 0, 0},
        {1, 1, 1},
        {0, 0, 0}
    },
    {
        {0, 1, 1},
        {0, 1, 0},
        {0, 1, 0}
    },
    {
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 1}
    },
    {
        {0, 1, 0},
        {0, 1, 0},
        {1, 1, 0}
    }
};

static const uint8_t block3[4][3][3] =
{
    {
        {1, 1, 0},
        {0, 1, 1},
        {0, 0, 0}
    },
    {
        {0, 0, 1},
        {0, 1, 1},
        {0, 1, 0}
    },
    {
        {0, 0, 0},
        {1, 1, 0},
        {0, 1, 1}
    },
    {
        {0, 1, 0},
        {1, 1, 0},
        {1, 0, 0}
    }
};

static const uint8_t block4[4][3][3] =
{
    {
        {1, 1, 1},
        {0, 0, 0},
        {0, 0, 0}
    },
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    },
    {
        {0, 0, 0},
        {0, 0, 0},
        {1, 1, 1}
    },
    {
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0}
    }
};

#endif /* BLOCKS_H */