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

bool is_passable_down(const uint8_t* screen_buffer, const block_t* block);
bool is_passable_left(const uint8_t* screen_buffer, const block_t* block);
bool is_passable_right(const uint8_t* screen_buffer, const block_t* block);
bool is_rotatable_clockwise(const uint8_t* screen_buffer, const block_t* block);
bool is_rotatable_anti_clockwise(const uint8_t* screen_buffer, const block_t* block);

#endif /* BLOCK_H */