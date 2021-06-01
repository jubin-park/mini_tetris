#include "block.h"

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

block_t generate_random_block(void)
{
    block_t block;

    block.angle = random() % ANGLE_SIZE;
    block.tile_of_zero_angle = BLOCK_TILES[(random() % BLOCK_COUNT) * BLOCK_HEIGHT * ANGLE_SIZE];

    const uint8_t* const p_block_tiles = block.tile_of_zero_angle + (block.angle * BLOCK_WIDTH * BLOCK_HEIGHT);

    int8_t real_height = 0;
    for (int8_t y = 0; y < BLOCK_HEIGHT; ++y) {
        for (int8_t x = 0; x < BLOCK_WIDTH; ++x) {
            if (1 == (p_block_tiles + y * BLOCK_WIDTH)[x]) {
                ++real_height;

                break;
            }
        }
    }

    block.x = 0;
    block.y = (-real_height);

    return block;
}

bool is_passable_down(const uint8_t* screen_buffer, const block_t* block)
{
    const uint8_t* const p_block_tiles = block->tile_of_zero_angle + (block->angle * BLOCK_WIDTH * BLOCK_HEIGHT);

    for (int8_t x = 0; x < BLOCK_WIDTH; ++x) {
        int8_t bottom_y = -1;

        for (int8_t y = BLOCK_HEIGHT - 1; y >= 0; --y) {
            if (1 == (p_block_tiles + y * BLOCK_WIDTH)[x]) {
                bottom_y = y;

                break;
            }
        }

        if (bottom_y >= 0) {
            const int8_t real_x = block->x + x;
            const int8_t real_y = block->y + bottom_y;

            if (real_y >= SCREEN_HEIGHT - 1
                || (screen_buffer[real_y + 1] & (1 << real_x)) > 0) {
                return false;
            }
        }
    }

    return true;
}

bool is_passable_left(const uint8_t* screen_buffer, const block_t* block)
{
    const uint8_t* const p_block_tiles = block->tile_of_zero_angle + (block->angle * BLOCK_WIDTH * BLOCK_HEIGHT);

    for (int8_t y = 0; y < BLOCK_HEIGHT; ++y) {
        int8_t leftmost_x = -1;

        for (int8_t x = 0; x < BLOCK_WIDTH; ++x) {
            if (1 == (p_block_tiles + y * BLOCK_WIDTH)[x]) {
                leftmost_x = x;

                break;
            }
        }

        if (leftmost_x >= 0) {
            const int8_t real_x = block->x + leftmost_x;
            const int8_t real_y = block->y + y;

            if (real_x <= 0
                || (screen_buffer[real_y] & (1 << (real_x - 1))) > 0) {
                return false;
            }
        }
    }

    return true;
}

bool is_passable_right(const uint8_t* screen_buffer, const block_t* block)
{
    const uint8_t* const p_block_tiles = block->tile_of_zero_angle + (block->angle * BLOCK_WIDTH * BLOCK_HEIGHT);

    for (int8_t y = 0; y < BLOCK_HEIGHT; ++y) {
        int8_t rightmost_x = -1;

        for (int8_t x = BLOCK_WIDTH - 1; x >= 0; --x) {
            if (1 == (p_block_tiles + y * BLOCK_WIDTH)[x]) {
                rightmost_x = x;

                break;
            }
        }

        if (rightmost_x >= 0) {
            const int8_t real_x = block->x + rightmost_x;
            const int8_t real_y = block->y + y;

            if (real_x >= SCREEN_WIDTH - 1
                || (screen_buffer[real_y] & (1 << (real_x + 1))) > 0) {
                return false;
            }
        }
    }

    return true;
}

bool is_rotatable_clockwise(const uint8_t* screen_buffer, const block_t* block)
{
    const angle_t next_angle = (block->angle + 1) % ANGLE_SIZE;
    const uint8_t* const p_block_tiles = block->tile_of_zero_angle + (next_angle * BLOCK_WIDTH * BLOCK_HEIGHT);
    
    for (int8_t y = 0; y < BLOCK_HEIGHT; ++y) {
        if (block->y + y >= 0) {
            for (int8_t x = 0; x < BLOCK_WIDTH; ++x) {
                if ((p_block_tiles + y * BLOCK_WIDTH)[x] > 0
                    && (screen_buffer[block->y + y] & (1 << (block->x + x))) > 0) {

                    return false;
                }
            }
        }
    }
    
    return true;
}

bool is_rotatable_anti_clockwise(const uint8_t* screen_buffer, const block_t* block)
{
    const angle_t next_angle = (block->angle - 1 + ANGLE_SIZE) % ANGLE_SIZE;
    const uint8_t* const p_block_tiles = block->tile_of_zero_angle + (next_angle * BLOCK_WIDTH * BLOCK_HEIGHT);
    
    for (int8_t y = 0; y < BLOCK_HEIGHT; ++y) {
        if (block->y + y >= 0) {
            for (int8_t x = 0; x < BLOCK_WIDTH; ++x) {
                if ((p_block_tiles + y * BLOCK_WIDTH)[x] > 0
                    && (screen_buffer[block->y + y] & (1 << (block->x + x))) > 0) {

                    return false;
                }
            }
        }
    }

    return true;
}