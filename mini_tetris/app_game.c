#define _POSIX_C_SOURCE (199309L)

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

bool g_is_game_running = true;

void signal_exit(int sig);
void display_matrix(const int fd);

int main(int argc, char* argv[])
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

    block_t now_block = {
        .pos = { 3, 0 },
        .angle = random() % ANGLE_SIZE,
        .tile_of_zero_angle = BLOCK_TILES + (random() % BLOCK_COUNT) * BLOCK_HEIGHT * ANGLE_SIZE;
    };
    
    uint32_t game_frame = 0;
    unsigned char old_buffer[ROW_COUNT] = { 0 };
    unsigned char switch_states[SWITCH_KEY_SIZE] = { 0 };

    struct timespec ts_sleep;
    ts_sleep.tv_sec = 1;
    ts_sleep.tv_nsec = 0L;

    srandom((unsigned int)time(NULL));

    while (g_is_game_running)
    {
        {// get switch key state
            size_t i;
            
            read(fd[DRIVER_PUSH_SWITCH], switch_states, sizeof(switch_states));

            for (i = 0; i < 3; ++i) {
                printf("%3d %3d %3d\n", switch_states[i * 3], switch_states[i * 3 + 1], switch_states[i * 3 + 2]);
            }
        }

        {// draw display_buffer
            unsigned char display_buffer[ROW_COUNT];
            memcpy(display_buffer, old_buffer, ROW_COUNT * sizeof(unsigned char));

            const uint8_t* random_block = now_block.tile_of_zero_angle + now_block.angle * BLOCK_HEIGHT;

            // draw block on display_buffer
            display_buffer[now_block.pos.y + 0] |= ((random_block + 0)[0] << 2 | (random_block + 0)[1] << 1 | (random_block + 0)[2]) << (6 - now_block.pos.x);
            display_buffer[now_block.pos.y + 1] |= ((random_block + 1)[0] << 2 | (random_block + 1)[1] << 1 | (random_block + 1)[2]) << (6 - now_block.pos.x);
            display_buffer[now_block.pos.y + 2] |= ((random_block + 2)[0] << 2 | (random_block + 2)[1] << 1 | (random_block + 2)[2]) << (6 - now_block.pos.x);

            // real drawing
            if (write(fd[DRIVER_DOT_MATRIX], display_buffer, ROW_COUNT * sizeof(unsigned char)) < 0) {
                fprintf(stderr, "write() error\n");
                
                goto lb_exit;
            }

            now_block.pos.y++;
            now_block.pos.y %= ROW_COUNT;
        }

        printf("frame = %4d\n", ++game_frame);
        display_matrix(fd[DRIVER_DOT_MATRIX]);
        puts("");

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
    unsigned char read_buf[ROW_COUNT];

    read(fd, read_buf, sizeof(read_buf));

    for (int i = 0; i < ROW_COUNT; ++i) {
        for (int b = 6; b >= 0; --b) {
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

bool is_collision_occured(const unsigned char* display_buffer, const block_t* block)
{
    return false;
}