#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

enum {
    MAX_BUTTON_SIZE = 9
};

static unsigned char s_quit = 0;

void user_signal(int sig);

int main(void)
{
    unsigned char switch_buf[MAX_BUTTON_SIZE] = {1};
    int cdev;

    cdev = open("/dev/csemad_push_switch", O_RDWR);
    if (cdev < 0) {
        printf("failed to open driver 'csemad_push_switch'\n");

        return -1;	
    }

    (void)signal(SIGINT, user_signal);

    printf("Press <crrl+c> to s_quit. \n");

    while(!s_quit) {
        unsigned char read_buf[MAX_BUTTON_SIZE];
        int i;
        int is_switch_changed = 0;

        usleep(400000);

        read(cdev, read_buf, sizeof(read_buf));

        for (i = 0; i < MAX_BUTTON_SIZE; ++i) {
            if (read_buf[i] != switch_buf[i]) {
                is_switch_changed = 1;
                break;
            }
        }

        memcpy(switch_buf, read_buf, sizeof(read_buf));

        if (is_switch_changed) {
            for (i = 0; i < 3; ++i) {
                printf("%3d %3d %3d\n", switch_buf[i * 3], switch_buf[i * 3 + 1], switch_buf[i * 3 + 2]);
            }
            puts("------------");
        }
    }

    close(cdev);
    
    return 0;
}

void user_signal(int sig)
{
    s_quit = 1;
}