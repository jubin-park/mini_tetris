#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
 
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>

#define CSEMAD_LED_MAJOR 260
#define CSEMAD_LED_NAME "csemad_led"
#define CSEMAD_LED_ADDRESS 0x08000016

static int g_ledport_usage = 0;
static unsigned char* g_csemad_led_addr;
 
static int csemad_led_open(struct inode* minode, struct file* mfile);
static int csemad_led_release(struct inode* minode, struct file* mfile);
static ssize_t csemad_led_read(struct file* inode, char* gdata, size_t length, loff_t* off_what);
static ssize_t csemad_led_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what);

static struct file_operations csemad_led_fops =
{
    .owner = THIS_MODULE,
    .open = csemad_led_open,
    .write = csemad_led_write,
    .read = csemad_led_read,
    .release = csemad_led_release, 
};

static int __init csemad_led_init(void)
{
    int chdev;
    printk(KERN_ALERT "csemad_led_init!\n");

    chdev = register_chrdev(CSEMAD_LED_MAJOR, CSEMAD_LED_NAME, &csemad_led_fops);
    if (0 != chdev) {
        printk(KERN_ALERT "The major number (%d) is busy\n", chdev);
        return 1;
    }

    g_csemad_led_addr = (unsigned char*)ioremap(CSEMAD_LED_ADDRESS, 1);
    return 0;
}

static void __exit csemad_led_exit(void)
{
    unregister_chrdev(CSEMAD_LED_MAJOR, CSEMAD_LED_NAME);
    iounmap(g_csemad_led_addr);

    printk(KERN_ALERT "csemad_led_exit!\n");
}

static int csemad_led_open(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "csemad_led_open!\n");

    if (g_ledport_usage != 0) {
        printk(KERN_ALERT "csemad_led_open failed: -EBUSY\n");
        return -EBUSY;
    }

    g_ledport_usage = 1;

    return 0;
}

static int csemad_led_release(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "csemad_led_release!\n");
    g_ledport_usage = 0;

    return 0;
}

static ssize_t csemad_led_read(struct file* inode, char* gdata, size_t length, loff_t* off_what)
{
    printk(KERN_ALERT "csemad_led_read!\n");

    if (0 != copy_to_user(gdata, g_csemad_led_addr, 1)) {
        printk(KERN_ALERT "csemad_led_read failed: -EFAULT\n");
        return -EFAULT;
    }

    printk(KERN_ALERT "g_csemad_led_addr:\t%p\t%d\n", g_csemad_led_addr, *g_csemad_led_addr);
    printk(KERN_ALERT "gdata:\t\t\t%p\t%d\n", gdata, *gdata);

    return length;
}

static ssize_t csemad_led_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what)
{
    printk(KERN_ALERT "csemad_led_write!\n");

    if (0 != copy_from_user(g_csemad_led_addr, gdata, 1)) {
        printk(KERN_ALERT "csemad_led_write failed: -EFAULT\n");
        return -EFAULT;
    }

    return length;
}

module_init(csemad_led_init);
module_exit(csemad_led_exit);