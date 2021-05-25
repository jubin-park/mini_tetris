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
#include "dot10x7/font.h"
#include "dot10x7/full.h"

typedef enum {
    DRIVER_LED,
    DRIVER_SEVEN_SEGMENT,
    DRIVER_DOT_MATRIX,
    DRIVER_LCD_TEXT,
    DRIVER_BUZZER,
    DRIVER_PUSH_SWITCH,
    DRIVER_SIZE
} driver_t;

const char* DRIVER_NAMES[DRIVER_SIZE] = {
    "/dev/csemad_led",
    "/dev/csemad_seven_segment",
    "/dev/csemad_dot_matrix",
    "/dev/csemad_lcd_text",
    "/dev/csemad_buzzer",
    "/dev/csemad_push_switch"
};

bool g_is_game_running = true;

void signal_exit(int sig);
void display_matrix(const int fd);

int main(int argc, char* argv[])
{
    int fd[DRIVER_SIZE] = { -1, -1, -1, -1, -1, -1 };
    fd[DRIVER_LED] = open(DRIVER_NAMES[DRIVER_LED], O_RDWR);
    fd[DRIVER_SEVEN_SEGMENT] = open(DRIVER_NAMES[DRIVER_SEVEN_SEGMENT], O_RDWR);
    fd[DRIVER_DOT_MATRIX] = open(DRIVER_NAMES[DRIVER_DOT_MATRIX], O_RDWR);
    fd[DRIVER_LCD_TEXT] = open(DRIVER_NAMES[DRIVER_LCD_TEXT], O_RDWR);
    fd[DRIVER_BUZZER] = open(DRIVER_NAMES[DRIVER_BUZZER], O_RDWR);
    fd[DRIVER_PUSH_SWITCH] = open(DRIVER_NAMES[DRIVER_PUSH_SWITCH], O_RDWR);
    {
        size_t i;
        bool has_error = false;

        for (i = 0; i < DRIVER_SIZE; ++i) {
            has_error |= fd[i] < 0;
            printf("Loading driver '%s' ... ", DRIVER_NAMES[i]);
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

    while (g_is_game_running)
    {
        unsigned char display_buffer[ROW_COUNT];
        memcpy(display_buffer, old_buffer, ROW_COUNT * sizeof(unsigned char));

        // TODO: draw block
        unsigned char line[3] = {
            (block1[ANGLE_0][0][0] << 2) | (block1[ANGLE_0][0][1] << 1) | block1[ANGLE_0][0][2],
            (block1[ANGLE_0][1][0] << 2) | (block1[ANGLE_0][1][1] << 1) | block1[ANGLE_0][1][2],
            (block1[ANGLE_0][2][0] << 2) | (block1[ANGLE_0][2][1] << 1) | block1[ANGLE_0][2][2]
        };

        display_buffer[now_block.y + 0] |= line[0] << (6 - now_block.x);
        display_buffer[now_block.y + 1] |= line[1] << (6 - now_block.x);
        display_buffer[now_block.y + 2] |= line[2] << (6 - now_block.x);

        // draw display_buffer
        if (write(fd[DRIVER_DOT_MATRIX], display_buffer, ROW_COUNT * sizeof(unsigned char)) < 0) {
            fprintf(stderr, "write() error\n");
            
            goto lb_exit;
        }
    
        now_block.y++;
        now_block.y %= 10;

        printf("frame = %4d\n", ++game_frame);
        display_buffer(fd[DRIVER_DOT_MATRIX]);
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