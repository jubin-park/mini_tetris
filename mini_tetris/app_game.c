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

#include "blocks.h"
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
            //goto lb_exit;
        }
    }

    (void)signal(SIGINT, signal_exit);

    block_status_t now_block = {
        .x = 3,
        .y = 0,
        .angle = ANGLE_0
    };
    uint32_t game_frame = 0;
    unsigned char old_buffer[ROW_COUNT] = { 0 };
    unsigned char switch_states[SWITCH_KEY_SIZE] = { 0 };

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

            // get block lines
            unsigned char line[3] = {
                (block1[ANGLE_0][0][0] << 2) | (block1[ANGLE_0][0][1] << 1) | block1[ANGLE_0][0][2],
                (block1[ANGLE_0][1][0] << 2) | (block1[ANGLE_0][1][1] << 1) | block1[ANGLE_0][1][2],
                (block1[ANGLE_0][2][0] << 2) | (block1[ANGLE_0][2][1] << 1) | block1[ANGLE_0][2][2]
            };
            // draw block on display_buffer
            display_buffer[now_block.y + 0] |= line[0] << (6 - now_block.x);
            display_buffer[now_block.y + 1] |= line[1] << (6 - now_block.x);
            display_buffer[now_block.y + 2] |= line[2] << (6 - now_block.x);

            // real drawing
            if (write(fd[DRIVER_DOT_MATRIX], display_buffer, ROW_COUNT * sizeof(unsigned char)) < 0) {
                fprintf(stderr, "write() error\n");
                
                goto lb_exit;
            }

            now_block.y++;
            now_block.y %= ROW_COUNT;
        }

        printf("frame = %4d\n", ++game_frame);
        display_matrix(fd[DRIVER_DOT_MATRIX]);
        puts("");

        usleep(1000000);
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