#include "dot_matrix_driver.h"

static int s_is_dot_matrix_using = 0;
static unsigned char* s_dot_matrix_addr;
 
static int __init dot_matrix_init(void)
{
    int chdev;
    printk(KERN_ALERT "dot_matrix_init!\n");

    chdev = register_chrdev(DOT_MATRIX_MAJOR, DOT_MATRIX_NAME, &dot_matrix_fops);
    if (0 != chdev) {
        printk(KERN_ALERT "The major number (%d) is busy\n", chdev);
        return 1;
    }

    s_dot_matrix_addr = (unsigned char*)ioremap(DOT_MATRIX_ADDRESS, MAPPING_BYTE_LENGTH);
    return 0;
}

static void __exit dot_matrix_exit(void)
{
    unregister_chrdev(DOT_MATRIX_MAJOR, DOT_MATRIX_NAME);
    iounmap(s_dot_matrix_addr);

    printk(KERN_ALERT "dot_matrix_exit!\n");
}

static int dot_matrix_open(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "dot_matrix_open!\n");

    if (s_is_dot_matrix_using != 0) {
        printk(KERN_ALERT "dot_matrix_open failed: -EBUSY\n");
        return -EBUSY;
    }

    s_is_dot_matrix_using = 1;

    return 0;
}

static int dot_matrix_release(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "dot_matrix_release!\n");
    s_is_dot_matrix_using = 0;

    return 0;
}

static ssize_t dot_matrix_read(struct file* inode, char* gdata, size_t length, loff_t* off_what)
{
    int i;
    printk(KERN_ALERT "dot_matrix_read!\n");

    for (i = 0; i < length; ++i) {
        if (0 != copy_to_user(gdata + i, s_dot_matrix_addr + i * 2, 1)) {
            printk(KERN_ALERT "dot_matrix_read failed: -EFAULT\n");
            return -EFAULT;
        }
    }

    printk(KERN_ALERT "s_dot_matrix_addr:\t%p\t%d\n", s_dot_matrix_addr, *s_dot_matrix_addr);

    return length;
}

static ssize_t dot_matrix_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what)
{
    int i;
    printk(KERN_ALERT "dot_matrix_write!\n");

    for (i = 0; i < length; ++i) {
        if (0 != copy_from_user(s_dot_matrix_addr + i * 2, gdata + i, 1)) {
            printk(KERN_ALERT "dot_matrix_write failed: -EFAULT\n");
            return -EFAULT;
        }
    }

    return length;
}

module_init(dot_matrix_init);
module_exit(dot_matrix_exit);