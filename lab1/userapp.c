#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

int main(void)
{
    int fd;
    if (0 > (fd = open("/dev/csemad", O_RDWR))) {
        printf("failed to open\n");
        return -1;			
    }

    printf("Device Driver Test Application\n");

    read(fd, 0, 0);
    ioctl(fd, 0, 0);
    close(fd);

    return 0;
}
