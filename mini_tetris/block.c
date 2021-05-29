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

bool is_collision_occured(const uint8_t* screen_buffer, const block_t* block)
{
    const uint8_t* const p_block_tiles = block->tile_of_zero_angle + (block->angle * BLOCK_WIDTH * BLOCK_HEIGHT);

    for (int8_t x = 0; x < BLOCK_WIDTH && block->x + x < SCREEN_WIDTH; ++x) {
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

            if (real_y >= SCREEN_HEIGHT - 1 || (screen_buffer[real_y + 1] & (1 << real_x))) {
                return true;
            }
        }
    }

    return false;
}