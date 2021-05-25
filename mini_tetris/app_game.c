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

int main(int argc, char* argv[])
{
    int fd[DRIVER_SIZE] = { -1, -1, -1, -1, -1, -1 };

    fd[DRIVER_LED] = open(DRIVER_NAMES[DRIVER_LED], O_RDWR);
    //fd[DRIVER_SEVEN_SEGMENT] = open(DRIVER_NAMES[DRIVER_SEVEN_SEGMENT], O_RDWR);
    fd[DRIVER_DOT_MATRIX] = open(DRIVER_NAMES[DRIVER_DOT_MATRIX], O_RDWR);
    //fd[DRIVER_LCD_TEXT] = open(DRIVER_NAMES[DRIVER_LCD_TEXT], O_RDWR);
    //fd[DRIVER_BUZZER] = open(DRIVER_NAMES[DRIVER_BUZZER], O_RDWR);
    //fd[DRIVER_PUSH_SWITCH] = open(DRIVER_NAMES[DRIVER_PUSH_SWITCH], O_RDWR);
    
    {
        size_t i;
        bool has_error = false;

        for (i = 0; i < DRIVER_SIZE; ++i) {
            has_error |= fd[i] < 0;
            if (fd[i] < 0) {
                fprintf(stderr, "Failed to open driver: '%s'\n", DRIVER_NAMES[i])
            } else {
                printf("Succeed to open driver: '%s'\n", DRIVER_NAMES[i])
            }
        }

        if (has_error) {
            goto lb_exit;
        }
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

static void print_bitflags(const bitflags_t* b)
{
    printf("D1|D2|D3|D4\n");
    printf("%2d|%2d|%2d|%2d\n", b->bits.b1, b->bits.b2, b->bits.b3, b->bits.b4);
    puts("------------");
    printf("D5|D6|D7|D8\n");
    printf("%2d|%2d|%2d|%2d\n\n", b->bits.b5, b->bits.b6, b->bits.b7, b->bits.b8);
}
