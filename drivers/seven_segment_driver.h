#ifndef SEVEN_SEGMENT_DRIVER_H
#define SEVEN_SEGMENT_DRIVER_H

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

#define SEVEN_SEGMENT_NAME ("csemad_seven_segment")

enum {
    SEVEN_SEGMENT_MAJOR = 261,
    SEVEN_SEGMENT_ADDRESS = 0x08000004,

    MAPPING_BYTE_LENGTH = 4,
};

static int __init seven_segment_init(void);
static void __exit seven_segment_exit(void);
static int seven_segment_open(struct inode* minode, struct file* mfile);
static int seven_segment_release(struct inode* minode, struct file* mfile);
static ssize_t seven_segment_read(struct file* inode, char* gdata, size_t length, loff_t* off_what);
static ssize_t seven_segment_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what);

static struct file_operations seven_segment_fops =
{
    .owner = THIS_MODULE,
    .open = seven_segment_open,
    .write = seven_segment_write,
    .read = seven_segment_read,
    .release = seven_segment_release, 
};

#endif /* SEVEN_SEGMENT_DRIVER_H */