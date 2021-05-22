#include "push_switch_driver.h"

static int g_push_switch_usage = 0;
static unsigned char* g_csemad_push_switch_addr;
 
static int __init csemad_push_switch_init(void)
{
    int chdev;
    printk(KERN_ALERT "csemad_push_switch_init!\n");

    chdev = register_chrdev(CSEMAD_PUSH_SWITCH_MAJOR, CSEMAD_PUSH_SWITCH_NAME, &csemad_push_switch_fops);
    if (0 != chdev) {
        printk(KERN_ALERT "The major number (%d) is busy\n", chdev);
        return 1;
    }

    g_csemad_push_switch_addr = (unsigned char*)ioremap(CSEMAD_PUSH_SWITCH_ADDRESS, MAPPING_BYTE_LENGTH);
    return 0;
}

static void __exit csemad_push_switch_exit(void)
{
    unregister_chrdev(CSEMAD_PUSH_SWITCH_MAJOR, CSEMAD_PUSH_SWITCH_NAME);
    iounmap(g_csemad_push_switch_addr);

    printk(KERN_ALERT "csemad_push_switch_exit!\n");
}

static int csemad_push_switch_open(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "csemad_push_switch_open!\n");

    if (g_push_switch_usage != 0) {
        printk(KERN_ALERT "csemad_push_switch_open failed: -EBUSY\n");
        return -EBUSY;
    }

    g_push_switch_usage = 1;

    return 0;
}

static int csemad_push_switch_release(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "csemad_push_switch_release!\n");
    g_push_switch_usage = 0;

    return 0;
}

static ssize_t csemad_push_switch_read(struct file* inode, char* gdata, size_t length, loff_t* off_what)
{
    int i;
    printk(KERN_ALERT "csemad_push_switch_read!\n");

    for (i = 0; i < length; ++i) {
        if (0 != copy_to_user(gdata + i, g_csemad_push_switch_addr + i * 2, 1)) {
            printk(KERN_ALERT "csemad_push_switch_read failed: -EFAULT\n");
            return -EFAULT;
        }
    }

    printk(KERN_ALERT "g_csemad_push_switch_addr:\t%p\t%d\n", g_csemad_push_switch_addr, *g_csemad_push_switch_addr);

    return length;
}

static ssize_t csemad_push_switch_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what)
{
    printk(KERN_ALERT "csemad_push_switch_write!\n");

    if (0 != copy_from_user(g_csemad_push_switch_addr, gdata, MAPPING_BYTE_LENGTH)) {
        printk(KERN_ALERT "csemad_push_switch_write failed: -EFAULT\n");
        return -EFAULT;
    }

    return length;
}

module_init(csemad_push_switch_init);
module_exit(csemad_push_switch_exit);