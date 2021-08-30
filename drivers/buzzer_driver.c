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

/*
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
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>

#define BUZZER_MAJOR 264// ioboard buzzer device major number
#define BUZZER_NAME "buzzer"// ioboard buzzer device name
#define BUZZER_ADDRESS 0x08000070 // pysical address

//Global variable
static int buzzer_port_usage = 0;
static unsigned char *buzzer_addr;
static unsigned char *demo_addr

// define functions...
ssize_t buzzer_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what);
ssize_t buzzer_read(struct file *inode, char *gdata, size_t length, loff_t *off_what);
int buzzer_open(struct inode *minode, struct file *mfile);
int buzzer_release(struct inode *minode, struct file *mfile);

// define file_operations structure 
struct file_operations buzzer_fops =
{
    .owner = THIS_MODULE,
    .open = buzzer_open,
    .write = buzzer_write,
    .read = buzzer_read,
    .release = buzzer_release,
};

// when buzzer device open ,call this function
int buzzer_open(struct inode *minode, struct file *mfile) 
{
    if(buzzer_port_usage != 0) 
        return -EBUSY;
    buzzer_port_usage = 1;
    return 0;
}
// when buzzer device close ,call this function
int buzzer_release(struct inode *minode, struct file *mfile) 
{
    buzzer_port_usage = 0;
    return 0;
}

// when write to buzzer device  ,call this function
ssize_t buzzer_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what) 
{
    unsigned char value;
    unsigned short int _s_value;
    const char *tmp = gdata;
    
    if (copy_from_user(&value, tmp, 1))
        return -EFAULT;

    _s_value = value & 0xF;

    outw(_s_value,(unsigned int)buzzer_addr);
    return length;
}

// when read to buzzer device  ,call this function
ssize_t buzzer_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) 
{
    unsigned char value;
    unsigned short int _s_value;
    char *tmp = gdata;
    
    _s_value = inw((unsigned int)buzzer_addr);
    value = _s_value & 0xF;
    
    if (copy_to_user(tmp, &value, 1))
        return -EFAULT;
    
    return length;
}

int __init buzzer_init(void)
{
    int result;
    result = register_chrdev(BUZZER_MAJOR, BUZZER_NAME, &buzzer_fops);
    
    if(result < 0) 
    {
        printk(KERN_WARNING "Can't get any major\n");
        return result;
    }
    
    buzzer_addr = ioremap(BUZZER_ADDRESS, 0x1);
    
    printk("init module, %s major number : %d\n", BUZZER_NAME, BUZZER_MAJOR);
    
    return 0;
}

void __exit buzzer_exit(void) 
{
    iounmap(buzzer_addr);
    unregister_chrdev(BUZZER_MAJOR, BUZZER_NAME);
}

module_init(buzzer_init);
module_exit(buzzer_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("csemad");
*/