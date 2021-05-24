#include "buzzer_driver.h"

static unsigned char s_usage;
static unsigned char* s_buzzer_addr;

static int __init buzzer_init(void)
{
    int chdev;
    printk(KERN_ALERT "buzzer_init!\n");

    chdev = register_chrdev(BUZZER_MAJOR, BUZZER_NAME, &buzzer_fops);
    if (0 != chdev) {
        printk(KERN_ALERT "The major number (%d) is busy\n", chdev);
        return 1;
    }

    s_buzzer_addr = (unsigned char*)ioremap(BUZZER_ADDRESS, MAPPING_BYTE_LENGTH);
    return 0;
}

static void __exit buzzer_exit(void)
{
    unregister_chrdev(BUZZER_MAJOR, BUZZER_NAME);
    iounmap(s_buzzer_addr);

    printk(KERN_ALERT "buzzer_exit!\n");
}

static int buzzer_open(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "buzzer_open!\n");

    if (s_usage != 0) {
        printk(KERN_ALERT "buzzer_open faibuzzer: -EBUSY\n");
        return -EBUSY;
    }

    s_usage = 1;

    return 0;
}

static int buzzer_release(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "buzzer_release!\n");
    s_usage = 0;

    return 0;
}

static ssize_t buzzer_read(struct file* inode, char* gdata, size_t length, loff_t* off_what)
{
    unsigned char value;
    unsigned short int _s_value;
    char *tmp = gdata;
    
    _s_value = inw((unsigned int)s_buzzer_addr);
    value = _s_value & 0xF;

    if (copy_to_user(tmp, &value, MAPPING_BYTE_LENGTH)) {
        return -EFAULT;
    }
    
    return length;
}

static ssize_t buzzer_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what)
{
    unsigned char value;
    unsigned short int _s_value;
    const char *tmp = gdata;

    if (copy_from_user(&value, tmp, MAPPING_BYTE_LENGTH)) {
        return -EFAULT;
    }

    _s_value = value & 0xF;
    outw(_s_value,(unsigned int)s_buzzer_addr);

    return length;
}

module_init(buzzer_init);
module_exit(buzzer_exit);