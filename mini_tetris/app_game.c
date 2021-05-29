#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "block.h"
#include "driver.h"
#include "switch_key.h"
#include "dot10x7/font.h"
#include "dot10x7/full.h"

#define DELAY_NANOSEC_PER_FRAME (100000000L)

enum {
    SCREEN_WIDTH = 7,
    SCREEN_HEIGHT = 10,
};

bool g_is_game_running = true;

void signal_exit(int sig);
void display_matrix(const int fd);
bool is_collision_occured(const uint8_t* screen_buffer, const block_t* block);
bool is_switch_key_pressed(const switch_key_t key);

int main()
{
    int fd[DRIVER_SIZE] = { -1, -1, -1, -1, -1, -1 };
    {
        bool has_error = false;
        size_t i;

        for (i = 0; i < DRIVER_SIZE; ++i) {
            printf("Loading driver '%s' ... ", DRIVER_NAMES[i]);

            fd[i] = open(DRIVER_NAMES[i], O_RDWR);
            has_error |= fd[i] < 0;

            puts(fd[i] < 0 ? "FAILED" : "success");
        }

        if (has_error) {
            printf("Try to execute it in sudo mode\n");

            goto lb_exit;
        }
    }

    (void)signal(SIGINT, signal_exit);
    srandom((unsigned int)time(NULL));

    block_t now_block = {
        .x = 0,
        .y = -3,
        .angle = random() % ANGLE_SIZE,
        .tile_of_zero_angle = BLOCK_TILES[(random() % BLOCK_COUNT) * BLOCK_HEIGHT * ANGLE_SIZE]
    };
    
    uint32_t frame_count = 0;
    uint8_t old_screen_buffer[SCREEN_HEIGHT] = { 0 };

    struct timespec ts_sleep;
    ts_sleep.tv_sec = 0;
    ts_sleep.tv_nsec = DELAY_NANOSEC_PER_FRAME;

    while (g_is_game_running)
    {
        // get switch key state            
        read(fd[DRIVER_PUSH_SWITCH], g_switch_states, sizeof(g_switch_states));

        if (is_switch_key_pressed(SWITCH_KEY_UP)) {
            //puts("UP");
        }
        if (is_switch_key_pressed(SWITCH_KEY_DOWN)) {
            //puts("DOWN");
        }
        if (is_switch_key_pressed(SWITCH_KEY_LEFT)) {
            //puts("LEFT");
            --now_block.x;
            if (now_block.x < 0) {
                now_block.x = 0;
            }
        }
        else if (is_switch_key_pressed(SWITCH_KEY_RIGHT)) {
            //puts("RIGHT");
            ++now_block.x;
            if (now_block.x + BLOCK_WIDTH >= SCREEN_WIDTH) {
                now_block.x = SCREEN_WIDTH - BLOCK_WIDTH;
            }
        }
        if (is_switch_key_pressed(SWITCH_KEY_OK_OR_ROTATE)) {
            //puts("OK");
        }

        // draw new_screen_buffer
        if (0 == frame_count % 10)
        {
            uint8_t new_screen_buffer[SCREEN_HEIGHT];
            memcpy(new_screen_buffer, old_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t));

            const uint8_t* p_block_tiles = now_block.tile_of_zero_angle + (now_block.angle * BLOCK_WIDTH * BLOCK_HEIGHT);
            printf("block #%2d\tangle %3d\n", (now_block.tile_of_zero_angle - BLOCK_TILES[0]) / (BLOCK_HEIGHT * ANGLE_SIZE * BLOCK_WIDTH), now_block.angle * 90);

            // draw block on new_screen_buffer
            for (int i = 0; i < BLOCK_HEIGHT; ++i) {
                if (now_block.y + i >= 0 && now_block.y + i < SCREEN_HEIGHT) {
                    new_screen_buffer[now_block.y + i] |= ((p_block_tiles + i * BLOCK_WIDTH)[2] << 2 | (p_block_tiles + i * BLOCK_WIDTH)[1] << 1 | (p_block_tiles + i * BLOCK_WIDTH)[0]) << now_block.x;    
                }
            }

            // real drawing
            if (write(fd[DRIVER_DOT_MATRIX], new_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t)) < 0) {
                fprintf(stderr, "write() error\n");
                
                goto lb_exit;
            }

            if (is_collision_occured(new_screen_buffer, &now_block)) {
                printf("collision occured\n");

                memcpy(old_screen_buffer, new_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t));

                now_block.x = 0;
                now_block.y = -3;
                now_block.angle = random() % ANGLE_SIZE;
                now_block.tile_of_zero_angle = BLOCK_TILES[(random() % BLOCK_COUNT) * BLOCK_HEIGHT * ANGLE_SIZE];
            }
            else {
                now_block.y++;
            }

            printf("frame = %4d\n", frame_count);
            display_matrix(fd[DRIVER_DOT_MATRIX]);
            puts("");
        }

        ++frame_count;
        nanosleep(&ts_sleep, NULL);
    }

lb_exit:
    {
        size_t i;
        for (i = 0; i < DRIVER_SIZE; ++i) {
            close(fd[i]);
        }
    }

    return 0;
}

void signal_exit(int sig)
{
    printf("<Exit Game>\tsig: %d\n", sig);
    g_is_game_running = false;
}

void display_matrix(const int fd)
{
    uint8_t read_buf[SCREEN_HEIGHT];

    read(fd, read_buf, sizeof(read_buf));

    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int b = SCREEN_WIDTH - 1; b >= 0; --b) {
            if ((1 << b) & read_buf[i]) {
                putchar('*');
            }
            else {
                putchar('.');
            }
        }
        putchar('\n');
    }
}

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

bool is_switch_key_pressed(const switch_key_t key)
{
    return g_switch_states[key] > 0;
}