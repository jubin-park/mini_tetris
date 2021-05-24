#include "push_switch_driver.h"

static unsigned char s_usage;
static unsigned char* s_push_switch_addr;
 
static int __init push_switch_init(void)
{
    int chdev;
    printk(KERN_ALERT "push_switch_init!\n");

    chdev = register_chrdev(PUSH_SWITCH_MAJOR, PUSH_SWITCH_NAME, &push_switch_fops);
    if (0 != chdev) {
        printk(KERN_ALERT "The major number (%d) is busy\n", chdev);
        return 1;
    }

    s_push_switch_addr = (unsigned char*)ioremap(PUSH_SWITCH_ADDRESS, MAPPING_BYTE_LENGTH);
    return 0;
}

static void __exit push_switch_exit(void)
{
    unregister_chrdev(PUSH_SWITCH_MAJOR, PUSH_SWITCH_NAME);
    iounmap(s_push_switch_addr);

    printk(KERN_ALERT "push_switch_exit!\n");
}

static int push_switch_open(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "push_switch_open!\n");

    if (s_usage != 0) {
        printk(KERN_ALERT "push_switch_open failed: -EBUSY\n");
        return -EBUSY;
    }

    s_usage = 1;

    return 0;
}

static int push_switch_release(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "push_switch_release!\n");
    s_usage = 0;

    return 0;
}

static ssize_t push_switch_read(struct file* inode, char* gdata, size_t length, loff_t* off_what)
{
    int i;
    printk(KERN_ALERT "push_switch_read!\n");

    for (i = 0; i < length; ++i) {
        if (0 != copy_to_user(gdata + i, s_push_switch_addr + i * 2, 1)) {
            printk(KERN_ALERT "push_switch_read failed: -EFAULT\n");
            return -EFAULT;
        }
    }

    printk(KERN_ALERT "s_push_switch_addr:\t%p\t%d\n", s_push_switch_addr, *s_push_switch_addr);

    return length;
}

static ssize_t push_switch_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what)
{
    printk(KERN_ALERT "push_switch_write!\n");

    if (0 != copy_from_user(s_push_switch_addr, gdata, MAPPING_BYTE_LENGTH)) {
        printk(KERN_ALERT "push_switch_write failed: -EFAULT\n");
        return -EFAULT;
    }

    return length;
}

module_init(push_switch_init);
module_exit(push_switch_exit);