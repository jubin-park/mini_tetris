#ifndef LED_DRIVER_H
#define LED_DRIVER_H

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

#define LED_NAME ("csemad_led")

enum {
    LED_MAJOR = 260,
    LED_ADDRESS = 0x08000016,
    
    MAPPING_BYTE_LENGTH = 20,

    ROW_COUNT = 10,
    COLUMN_COUNT = 7,
};

static int led_open(struct inode* minode, struct file* mfile);
static int led_release(struct inode* minode, struct file* mfile);
static ssize_t led_read(struct file* inode, char* gdata, size_t length, loff_t* off_what);
static ssize_t led_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what);

static struct file_operations led_fops =
{
    .owner = THIS_MODULE,
    .open = led_open,
    .write = led_write,
    .read = led_read,
    .release = led_release, 
};

#endif /* LED_DRIVER_H */