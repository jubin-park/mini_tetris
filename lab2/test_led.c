#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

typedef union {
    unsigned char val;
    struct {
        unsigned char b8 : 1;
        unsigned char b7 : 1;
        unsigned char b6 : 1;
        unsigned char b5 : 1;
        unsigned char b4 : 1;
        unsigned char b3 : 1;
        unsigned char b2 : 1;
        unsigned char b1 : 1;
    } bits;
} bitflags_t;

static void print_bitflags(const bitflags_t* b);

int main(int argc, char** argv)
{
    int fd;

    bitflags_t set_led_bits = {0};
    bitflags_t get_led_bits = {0};

    if (2 != argc) {
        fprintf(stderr, "Usage: ./led_test <range of 1-255>\n");
        return -1;
    }

    {
        const char* p = argv[1];
        while (*p != '\0') {
            set_led_bits.val *= 10;
            set_led_bits.val += *p - '0';
            ++p;
        }
    }

    fd = open("/dev/csemad_led", O_RDWR);
    if (fd < 0) {
        printf("failed to open driver 'csemad_led'\n");

        return -1;			
    }

    printf("Start to loading LED Driver ...\n");

    printf("Set to LED Value: %d\n", set_led_bits.val);
    write(fd, &set_led_bits.val, 1);

    read(fd, &get_led_bits.val, 1);
    printf("Current LED Value : %d\n", get_led_bits.val);
    print_bitflags(&get_led_bits);

    close(fd);

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
