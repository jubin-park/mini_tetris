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

#include "config.h"
#include "block.h"
#include "driver.h"
#include "switch_key.h"
#include "dot10x7/font.h"
#include "dot10x7/full.h"

extern const uint8_t BLOCK_TILES[BLOCK_COUNT * BLOCK_HEIGHT * ANGLE_SIZE][BLOCK_WIDTH];
extern uint8_t g_score_text[SEVEN_SEGMENT_DATA_LENGTH];
extern uint8_t g_now_switch_states[SWITCH_KEY_SIZE];
extern uint8_t g_old_switch_states[SWITCH_KEY_SIZE];

bool g_is_game_running = true;
uint32_t g_score;

void signal_exit(int sig);
void display_matrix(const uint8_t* screen_buffer);
bool render_matrix(const uint8_t* screen_buffer);

int main(void)
{
    if (!open_drivers()) {
        fprintf(stderr, "Try to execute it in sudo mode\n");

        goto lb_exit;
    }

    clear_drivers();
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

    struct timespec ts_sleep = {
        .tv_sec = 0,
        .tv_nsec = DELAY_NANOSEC_PER_FRAME
    };

    while (g_is_game_running) {

        bool redraw = false;

        {// get switch key state
            if (read(get_driver_file_descriptor(DRIVER_PUSH_SWITCH), g_now_switch_states, sizeof(g_now_switch_states)) < 0) {
                fprintf(stderr, "Failed to read switch key\n");

                goto lb_exit;
            }
        
            if (is_switch_key_triggered(SWITCH_KEY_UP)) {
                puts("UP");
            }
            if (is_switch_key_triggered(SWITCH_KEY_DOWN)) {
                puts("DOWN");
                while (is_passable_down(old_screen_buffer, &now_block)) {
                    ++now_block.y;
                    redraw = true;
                }
            }
            if (is_switch_key_triggered(SWITCH_KEY_LEFT)) {
                puts("LEFT");
                if (is_passable_left(old_screen_buffer, &now_block)) {
                    ++now_block.x;
                    redraw = true;
                }
            }
            else if (is_switch_key_triggered(SWITCH_KEY_RIGHT)) {
                puts("RIGHT");
                if (is_passable_right(old_screen_buffer, &now_block)) {
                    --now_block.x;
                    redraw = true;
                }
            }
            if (is_switch_key_triggered(SWITCH_KEY_OK_OR_ROTATE)) {
                puts("ROTATE");
                if (is_rotatable_clockwise(old_screen_buffer, &now_block)) {
                    now_block.angle = (now_block.angle + 1) % ANGLE_SIZE;
                    redraw = true;
                }
            }

            memcpy(g_old_switch_states, g_now_switch_states, sizeof(g_now_switch_states));
        }

        if (redraw) { // redraw when block moved or rotated
            printf("x: %2d\ty: %2d\n", now_block.x, now_block.y);

            uint8_t new_screen_buffer[SCREEN_HEIGHT];
            memcpy(new_screen_buffer, old_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t));

            const uint8_t* p_block_tiles = now_block.tile_of_zero_angle + (now_block.angle * BLOCK_WIDTH * BLOCK_HEIGHT);

            // draw block on new_screen_buffer
            for (int i = 0; i < BLOCK_HEIGHT; ++i) {
                if (now_block.y + i >= 0 && now_block.y + i < SCREEN_HEIGHT) {
                    uint8_t line = (p_block_tiles + i * BLOCK_WIDTH)[2] << 2 | (p_block_tiles + i * BLOCK_WIDTH)[1] << 1 | (p_block_tiles + i * BLOCK_WIDTH)[0];
                    if (now_block.x >= 0) {
                        line <<= now_block.x;
                    } else {
                        line <<= (-now_block.x);
                    }
                    new_screen_buffer[now_block.y + i] |= line;
                }
            }

            // real drawing
            render_matrix(new_screen_buffer);
        }

        // draw new_screen_buffer per a frame
        if (0 == frame_count % DRAWING_DELAY_FRAME_COUNT) {
            uint8_t new_screen_buffer[SCREEN_HEIGHT];
            memcpy(new_screen_buffer, old_screen_buffer, SCREEN_HEIGHT * sizeof(uint8_t));

            const uint8_t* p_block_tiles = now_block.tile_of_zero_angle + (now_block.angle * BLOCK_WIDTH * BLOCK_HEIGHT);
            printf("block #%d\tx: %2d y: %2d\tangle: %3d\tframe: %6d\n",
                (now_block.tile_of_zero_angle - BLOCK_TILES[0]) / (BLOCK_HEIGHT * ANGLE_SIZE * BLOCK_WIDTH),
                now_block.x,
                now_block.y,
                now_block.angle * 90,
                frame_count);

            // draw block on new_screen_buffer
            for (int i = 0; i < BLOCK_HEIGHT; ++i) {
                if (now_block.y + i >= 0 && now_block.y + i < SCREEN_HEIGHT) {
                    uint8_t line = (p_block_tiles + i * BLOCK_WIDTH)[2] << 2 | (p_block_tiles + i * BLOCK_WIDTH)[1] << 1 | (p_block_tiles + i * BLOCK_WIDTH)[0];
                    if (now_block.x >= 0) {
                        line <<= now_block.x;
                    } else {
                        line <<= (-now_block.x);
                    }
                    new_screen_buffer[now_block.y + i] |= line;
                }
            }

            // real drawing
            render_matrix(new_screen_buffer);

            if (!is_passable_down(new_screen_buffer, &now_block)) {
                printf("collision occured\n");

                if ((new_screen_buffer[0] & 0x7f) > 0) {
                    puts("== GAME OVER ==");
                    goto lb_exit;
                }

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

                    update_score_text(g_score);

                    // when line is removed, redraw again
                    render_matrix(new_screen_buffer);
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
    clear_drivers();
    close_drivers();

    puts("Bye bye ...");

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
        for (int8_t x = 0; x < SCREEN_WIDTH; ++x) {
            putchar(((1 << x) & screen_buffer[y]) ? '*' : '.');
        }
        putchar('\n');
    }
}

bool render_matrix(const uint8_t* screen_buffer)
{
    uint8_t flip_buffer[SCREEN_HEIGHT];
    
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        const uint8_t bits = screen_buffer[i];
        flip_buffer[i] = ((bits & 1) << 7 | (bits & 128) >> 7
                        | (bits & 2) << 5 | (bits & 64) >> 5
                        | (bits & 4) << 3 | (bits & 32) >> 3
                        | (bits & 8) << 1 | (bits & 16) >> 1) >> 1;
    }

    if (write(get_driver_file_descriptor(DRIVER_DOT_MATRIX), flip_buffer, SCREEN_HEIGHT * sizeof(uint8_t)) < 0) {
        fprintf(stderr, "render_matrix() error\n");
        
        return false;
    }

    return true;
}
