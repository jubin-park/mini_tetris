#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUFF 32
#define LINE_BUFF 16
#define LCD_DEVICE "/dev/csemad_lcd_text"

int main(int argc, char **argv)
{
    int i;
    int dev;
    int str_size;
    int chk_size;
    unsigned char string[32];

    memset(string, 0, sizeof(string));
    
    if (argc < 2 || argc > 3) 
    {
        printf("Invalid Value Arguments!\n");
        return -1;
    }
    
    if (strlen(argv[1]) > LINE_BUFF || strlen(argv[2]) > LINE_BUFF)
    {
        printf("16 alphanumeric characters on a line!\n");
        return -1;
    }
    
    dev = open(LCD_DEVICE, O_WRONLY);
    
    printf("fild open success!\n");

    if (dev < 0) 
    {
        printf("Device open error : %s\n", LCD_DEVICE);
        return -1;
    }
    
    str_size = strlen(argv[1]);
    
    printf("str_size = %d\n", str_size);
    
    if (str_size > 0) 
    {
        size_t i;

        for (i = 0; i < str_size; i++) {
            string[i] = argv[1][i];
        }
        string[str_size] = '\0';

        memset(string + str_size, ' ', LINE_BUFF - str_size);
    }
    
    str_size = strlen(argv[2]);

    if (str_size > 0) 
    {
        size_t i;

        for (i = 0; i < str_size; i++) {
            string[i + LINE_BUFF] = argv[2][i];
        }
        string[LINE_BUFF + str_size] = '\0';
        
        memset(string + LINE_BUFF + str_size, ' ', LINE_BUFF - str_size);
    }
    

    printf("strings = %s\n", string);
    printf("write\n");
    write(dev, string, MAX_BUFF);
    close(dev);
    
    return(0);
}