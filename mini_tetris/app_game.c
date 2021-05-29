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
#include "config.h"
#include "driver.h"
#include "switch_key.h"
#include "dot10x7/font.h"
#include "dot10x7/full.h"

bool g_is_game_running = true;
uint32_t g_score;
uint8_t g_score_text[4];

void signal_exit(int sig);
void display_matrix(const uint8_t* screen_buffer);
bool is_collision_occured(const uint8_t* screen_buffer, const block_t* block);
bool is_switch_key_pressed(const switch_key_t key);
bool is_switch_key_triggered(const switch_key_t key);
void update_score_text(const int fd, const uint32_t original_score);

int main()
{
    int fd[DRIVER_SIZE] = { -1, -1, -1, -1, -1, -1 };
    {
        bool has_error = false;

        for (size_t i = 0; i < DRIVER_SIZE; ++i) {
            printf("Loading driver '%s' ... ", DRIVER_NAMES[i]);

            fd[i] = open(DRIVER_NAMES[i], O_RDWR);
            has_error |= fd[i] < 0;

            puts(fd[i] < 0 ? "FAILED" : "success");
        }

        if (has_error) {
            fprintf(stderr, "Try to execute it in sudo mode\n");

            goto lb_exit;
        }

        memset(g_score_text, 0x0, 4);
        write(fd[DRIVER_SEVEN_SEGMENT], g_score_text, 4);
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
old_screen_buffer[SCREEN_HEIGHT - 1] = 0x77;
old_screen_buffer[SCREEN_HEIGHT - 2] = 0x77;

    struct timespec ts_sleep = {
        .tv_sec = 0,
        .tv_nsec = DELAY_NANOSEC_PER_FRAME
    };

    while (g_is_game_running) {

        {// get switch key state
            if (read(fd[DRIVER_PUSH_SWITCH], g_now_switch_states, sizeof(g_now_switch_states)) < 0) {
                fprintf(stderr, "Failed to read switch key\n");

                goto lb_exit;
            }
        
            if (is_switch_key_triggered(SWITCH_KEY_UP)) {
                puts("UP");
            }
            if (is_switch_key_triggered(SWITCH_KEY_DOWN)) {
                puts("DOWN");
            }
            if (is_switch_key_triggered(SWITCH_KEY_LEFT)) {
                puts("LEFT");
                --now_block.x;
                if (now_block.x < 0) {
                    now_block.x = 0;
                }
            }
            else if (is_switch_key_triggered(SWITCH_KEY_RIGHT)) {
                puts("RIGHT");
                ++now_block.x;
                if (now_block.x + BLOCK_WIDTH >= SCREEN_WIDTH) {
                    now_block.x = SCREEN_WIDTH - BLOCK_WIDTH;
                }
            }
            if (is_switch_key_triggered(SWITCH_KEY_OK_OR_ROTATE)) {
                puts("OK");
            }

            memcpy(g_old_switch_states, g_now_switch_states, sizeof(g_now_switch_states));
        }

        // draw new_screen_buffer
        if (0 == frame_count % DRAWING_DELAY_FRAME_COUNT) {
            uint8_t new_screen_buffer[SCREEN_HEIGHT];
            memcpy(new_screen_buffer, old_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t));

            const uint8_t* p_block_tiles = now_block.tile_of_zero_angle + (now_block.angle * BLOCK_WIDTH * BLOCK_HEIGHT);
            printf("block #%d\tangle: %3d\tframe: %6d\n",
                (now_block.tile_of_zero_angle - BLOCK_TILES[0]) / (BLOCK_HEIGHT * ANGLE_SIZE * BLOCK_WIDTH),
                now_block.angle * 90,
                frame_count);

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

                now_block.x = 0;
                now_block.y = -3;
                now_block.angle = random() % ANGLE_SIZE;
                now_block.tile_of_zero_angle = BLOCK_TILES[(random() % BLOCK_COUNT) * BLOCK_HEIGHT * ANGLE_SIZE];

                int8_t removed_height = 0;
                
                while (true) {
                    bool is_line_found = false;

                    for (int8_t h = SCREEN_HEIGHT - 1; h >= 0; --h) {
                        if ((new_screen_buffer[h] & 0xff) == 0x7f) {
                            ++removed_height;
                            
                            for (int8_t i = h; i >= 1; --i) {
                                new_screen_buffer[i] = new_screen_buffer[i - 1];
                            }

                            is_line_found = true;

                            break;
                        }
                    }

                    if (!is_line_found) {
                        break;
                    }
                }

                if (removed_height > 0) {
                    memset(new_screen_buffer, 0x0, removed_height * sizeof(uint8_t));
                    g_score += removed_height * DEFAULT_SCORE;

                    update_score_text(fd[DRIVER_SEVEN_SEGMENT], g_score);
                }

                memcpy(old_screen_buffer, new_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t));
            }
            else {
                now_block.y++;
            }

            display_matrix(new_screen_buffer);
        }

        nanosleep(&ts_sleep, NULL);
        ++frame_count;
    }

lb_exit:
    for (size_t i = 0; i < DRIVER_SIZE; ++i) {
        close(fd[i]);
    }

    return 0;
}

void signal_exit(int sig)
{
    printf("<Exit Game>\tsig: %d\n", sig);
    g_is_game_running = false;
}

void display_matrix(const uint8_t* screen_buffer)
{
    for (int8_t y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int8_t x = SCREEN_WIDTH - 1; x >= 0; --x) {
            putchar(((1 << x) & screen_buffer[y]) ? '*' : '.');
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
    return g_now_switch_states[key] > 0;
}

bool is_switch_key_triggered(const switch_key_t key)
{
    if (0 == g_old_switch_states[key] && g_now_switch_states[key] > 0) {
        g_old_switch_states[key] = 1;

        return true;
    }

    return false;
}

void update_score_text(const int fd, const uint32_t original_score)
{
    uint32_t score = original_score;
    uint8_t* p = g_score_text + sizeof(g_score_text);

    memset(g_score_text, 0, sizeof(g_score_text));

    if (score > 9999) {
        score = 9999;
    }

    do {
        *--p = score % 10;
        score /= 10;
    } while (score > 0 && p >= g_score_text);

    if (write(fd, g_score_text, sizeof(g_score_text)) < 0) {
        fprintf(stderr, "update_score_text Error\n");
    }
}