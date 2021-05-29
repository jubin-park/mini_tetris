#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

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
    /* SIZE */
    ANGLE_SIZE
} angle_t;

typedef struct {
    int8_t x;
    int8_t y;
    angle_t angle;
    const uint8_t* tile_of_zero_angle;
} block_t;

bool is_collision_occured(const uint8_t* screen_buffer, const block_t* block);

#endif /* BLOCK_H */