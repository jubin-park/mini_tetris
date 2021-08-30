#include "seven_segment_driver.h"

static unsigned s_usage;
static unsigned char* s_seven_segment_addr;

static int __init seven_segment_init(void)
{
    int chdev;
    printk(KERN_ALERT "seven_segment_init!\n");

    chdev = register_chrdev(SEVEN_SEGMENT_MAJOR, SEVEN_SEGMENT_NAME, &seven_segment_fops);
    if (0 > chdev) {
        printk(KERN_ALERT "The major number (%d) is busy\n", chdev);
        return 1;
    }

    s_seven_segment_addr = (unsigned char*)ioremap(SEVEN_SEGMENT_ADDRESS, MAPPING_BYTE_LENGTH);
    return 0;
}

static void __exit seven_segment_exit(void)
{
    unregister_chrdev(SEVEN_SEGMENT_MAJOR, SEVEN_SEGMENT_NAME);
    iounmap(s_seven_segment_addr);

    printk(KERN_ALERT "seven_segment_exit!\n");
}

static int seven_segment_open(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "seven_segment_open!\n");

    if (s_usage != 0) {
        printk(KERN_ALERT "seven_segment_open faiseven_segment: -EBUSY\n");
        return -EBUSY;
    }

    s_usage = 1;

    return 0;
}

static int seven_segment_release(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "seven_segment_release!\n");
    s_usage = 0;

    return 0;
}

static ssize_t seven_segment_read(struct file* inode, char* gdata, size_t length, loff_t* off_what)
{
    unsigned char value[MAPPING_BYTE_LENGTH];
    unsigned short int value_short = 0;
    char *tmp = gdata;
    
    value_short = inw((unsigned int)s_seven_segment_addr);
    
    value[0] =(value_short >> 12) & 0xF;
    value[1] =(value_short >> 8) & 0xF;
    value[2] =(value_short >> 4) & 0xF;
    value[3] = value_short & 0xF;

    if (copy_to_user(tmp, value, MAPPING_BYTE_LENGTH)) {
        return -EFAULT;
    }

    return length;
}

static ssize_t seven_segment_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what)
{
    unsigned char value[MAPPING_BYTE_LENGTH];
    unsigned short int value_short = 0;

    const char *tmp = gdata;

    if (copy_from_user(&value, tmp, MAPPING_BYTE_LENGTH)) {
        return -EFAULT;
    }

    value_short = value[0] << 12 | value[1] << 8 | value[2] << 4 | value[3];
    outw(value_short, (unsigned int)s_seven_segment_addr);

    return length;
}

module_init(seven_segment_init);
module_exit(seven_segment_exit);