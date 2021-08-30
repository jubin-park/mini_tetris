#ifndef PUSH_SWITCH_DRIVER_H
#define PUSH_SWITCH_DRIVER_H

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

#define PUSH_SWITCH_NAME ("push_switch")

enum {
    PUSH_SWITCH_MAJOR = 265,
    PUSH_SWITCH_ADDRESS = 0x08000050,

    MAPPING_BYTE_LENGTH = 9,
};

static int __init push_switch_init(void);
static void __exit push_switch_exit(void);
static int push_switch_open(struct inode* minode, struct file* mfile);
static int push_switch_release(struct inode* minode, struct file* mfile);
static ssize_t push_switch_read(struct file* inode, char* gdata, size_t length, loff_t* off_what);
static ssize_t push_switch_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what);

static struct file_operations push_switch_fops =
{
    .owner = THIS_MODULE,
    .open = push_switch_open,
    .write = push_switch_write,
    .read = push_switch_read,
    .release = push_switch_release, 
};

#endif // PUSH_SWITCH_DRIVER_H