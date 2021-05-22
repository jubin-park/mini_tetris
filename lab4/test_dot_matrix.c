#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include "dot_matrix_font.h"

int main(int argc, char** argv)
{
    int cdev;
    int number = 0;
    char read_buf[ROW_COUNT];

    if (2 != argc) {
        fprintf(stderr, "[Usage] ./test_dot_matrix [0-9]\n");
        return -1;
    }

    if (sscanf(argv[1], "%d", &number) != 1 || number < 0 || number > 9) {
        fprintf(stderr, "Please input number [0-9]\n");
        return -1;
    }

    cdev = open("/dev/csemad_dot_matrix", O_RDWR);
    if (cdev < 0) {
        printf("failed to open driver 'csemad_dot_matrix'\n");

        return -1;	
    }

    if (write(cdev, g_dot_matrix_font[number], ROW_COUNT * sizeof(unsigned char)) < 0) {
        fprintf(stderr, "write() error\n");
        
        goto lb_exit;
    }

    read(cdev, read_buf, sizeof(read_buf));

    for (int i = 0; i < ROW_COUNT; ++i) {
        putchar('x');
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

lb_exit:
    close(cdev);
    
    return 0;
}