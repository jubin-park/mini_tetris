#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#define MAX_DIGIT 4
#define SEVEN_SEGMENT_DEVICE "/dev/csemad_seven_segment"

int main(int argc, char **argv)
{
    int dev;
    unsigned char data[4];
    unsigned char retval;
    int i;
    int str_size;

    memset(data, 0, sizeof(data));
    
    if (argc != 2) 
    {
        printf("please input the parameter! \n");
        printf("ex)./seven_segment_test_app <int value>\n");
        return -1;
    }
    
    str_size = (strlen(argv[1]));
    
    if (str_size > MAX_DIGIT)
    {
        printf("Warning! 4 Digit number only!\n");
        str_size = MAX_DIGIT;
    }
    
    for (i = 0; i < str_size; i++)
    {
        if ((argv[1][i] < 0x30) || (argv[1][i]) > 0x39) 
        {
            printf("Error! Invalid Value!\n");
            return -1;
        }
        
        data[i] = argv[1][i] - 0x30;
    }
    
    dev = open(SEVEN_SEGMENT_DEVICE, O_RDWR);
    
    if (dev < 0) 
    {
        printf("Device open error : %s\n", SEVEN_SEGMENT_DEVICE);
        exit(1);
    }
    
    retval = write(dev, &data, 4);
    
    if (retval < 0) 
    {
        printf("Write Error!\n");
        return -1;
    }
    
    memset(data, 0, sizeof(data));
    
    sleep(1);
    
    retval = read(dev, &data, 4);
    
    if (retval < 0) 
    {
        printf("Read Error!\n");
        return -1;
    }
    
    printf("Current FND Value : ");

    for (i = 0; i < str_size; i++)
        printf("%d", data[i]);
    
    printf("\n");
    
    close(dev);
    return(0);
}