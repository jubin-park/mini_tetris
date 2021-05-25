#ifndef BLOCKS_H
#define BLCOKS_H

#include <stdint.h>

static const uint8_t shape1_0[3][3] = {
    {1, 1, 1},
    {0, 1, 0},
    {0, 0, 0}
};

static const uint8_t shape1_90[3][3] = {
    {0, 0, 1},
    {0, 1, 1},
    {0, 0, 1}
};

static const uint8_t shape1_180[3][3] = {
    {0, 0, 0},
    {0, 1, 0},
    {1, 1, 1}
};

static const uint8_t shape1_270[3][3] = {
    {1, 0, 0},
    {1, 1, 0},
    {1, 0, 0}
};

static const uint8_t shape2_0[3][3] = {
    {1, 0, 0},
    {1, 1, 1},
    {0, 0, 0}
};

static const uint8_t shape2_90[3][3] = {
    {1, 1, 0},
    {1, 0, 0},
    {1, 0, 0}
};

static const uint8_t shape2_180[3][3] = {
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 0}
};

static const uint8_t shape2_270[3][3] = {
    {0, 0, 1},
    {0, 0, 1},
    {0, 1, 1}
};

static const uint8_t shape3_0[3][3] = {
    {1, 1, 0},
    {0, 1, 1},
    {0, 0, 0}
};

static const uint8_t shape3_90[3][3] = {
    {0, 0, 1},
    {0, 1, 1},
    {0, 1, 0}
};

static const uint8_t shape3_180[3][3] = {
    {0, 0, 0},
    {1, 1, 0},
    {0, 1, 1}
};

static const uint8_t shape3_270[3][3] = {
    {0, 1, 0},
    {1, 1, 0},
    {1, 0, 0}
};

static const uint8_t shape4_0[3][3] = {
    {1, 1, 1},
    {0, 0, 0},
    {0, 0, 0}
};

static const uint8_t shape4_90[3][3] = {
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1}
};

static const uint8_t shape4_180[3][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {1, 1, 1}
};

static const uint8_t shape4_270[3][3] = {
    {1, 0, 0},
    {1, 0, 0},
    {1, 0, 0}
};

#endif /* BLOCKS_H */