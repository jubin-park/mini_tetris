#include "led_driver.h"

static unsigned char s_usage;
static unsigned char* s_led_addr;

static int __init led_init(void)
{
    int chdev;
    printk(KERN_ALERT "led_init!\n");

    chdev = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
    if (0 != chdev) {
        printk(KERN_ALERT "The major number (%d) is busy\n", chdev);
        return 1;
    }

    s_led_addr = (unsigned char*)ioremap(LED_ADDRESS, MAPPING_BYTE_LENGTH);
    return 0;
}

static void __exit led_exit(void)
{
    unregister_chrdev(LED_MAJOR, LED_NAME);
    iounmap(s_led_addr);

    printk(KERN_ALERT "led_exit!\n");
}

static int led_open(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "led_open!\n");

    if (s_usage != 0) {
        printk(KERN_ALERT "led_open failed: -EBUSY\n");
        return -EBUSY;
    }

    s_usage = 1;

    return 0;
}

static int led_release(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "led_release!\n");
    s_usage = 0;

    return 0;
}

static ssize_t led_read(struct file* inode, char* gdata, size_t length, loff_t* off_what)
{
    printk(KERN_ALERT "led_read!\n");

    if (0 != copy_to_user(gdata, s_led_addr, MAPPING_BYTE_LENGTH)) {
        printk(KERN_ALERT "led_read failed: -EFAULT\n");
        return -EFAULT;
    }

    printk(KERN_ALERT "s_led_addr:\t%p\t%d\n", s_led_addr, *s_led_addr);
    printk(KERN_ALERT "gdata:\t\t\t%p\t%d\n", gdata, *gdata);

    return length;
}

static ssize_t led_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what)
{
    printk(KERN_ALERT "led_write!\n");

    if (0 != copy_from_user(s_led_addr, gdata, MAPPING_BYTE_LENGTH)) {
        printk(KERN_ALERT "led_write failed: -EFAULT\n");
        return -EFAULT;
    }

    return length;
}

module_init(led_init);
module_exit(led_exit);