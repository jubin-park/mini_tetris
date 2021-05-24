#include "led_driver.h"

static int g_ledport_usage;
static unsigned char* g_led_addr;

static int __init led_init(void)
{
    int chdev;
    printk(KERN_ALERT "led_init!\n");

    chdev = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
    if (0 != chdev) {
        printk(KERN_ALERT "The major number (%d) is busy\n", chdev);
        return 1;
    }

    g_led_addr = (unsigned char*)ioremap(LED_ADDRESS, 1);
    return 0;
}

static void __exit led_exit(void)
{
    unregister_chrdev(LED_MAJOR, LED_NAME);
    iounmap(g_led_addr);

    printk(KERN_ALERT "led_exit!\n");
}

static int led_open(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "led_open!\n");

    if (g_ledport_usage != 0) {
        printk(KERN_ALERT "led_open failed: -EBUSY\n");
        return -EBUSY;
    }

    g_ledport_usage = 1;

    return 0;
}

static int led_release(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "led_release!\n");
    g_ledport_usage = 0;

    return 0;
}

static ssize_t led_read(struct file* inode, char* gdata, size_t length, loff_t* off_what)
{
    printk(KERN_ALERT "led_read!\n");

    if (0 != copy_to_user(gdata, g_led_addr, 1)) {
        printk(KERN_ALERT "led_read failed: -EFAULT\n");
        return -EFAULT;
    }

    printk(KERN_ALERT "g_led_addr:\t%p\t%d\n", g_led_addr, *g_led_addr);
    printk(KERN_ALERT "gdata:\t\t\t%p\t%d\n", gdata, *gdata);

    return length;
}

static ssize_t led_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what)
{
    printk(KERN_ALERT "led_write!\n");

    if (0 != copy_from_user(g_led_addr, gdata, 1)) {
        printk(KERN_ALERT "led_write failed: -EFAULT\n");
        return -EFAULT;
    }

    return length;
}

module_init(led_init);
module_exit(led_exit);