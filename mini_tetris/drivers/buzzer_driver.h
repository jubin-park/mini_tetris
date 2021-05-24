#ifndef BUZZER_DRIVER_H
#define BUZZER_DRIVER_H

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

#define BUZZER_NAME ("csemad_buzzer")

enum {
    BUZZER_MAJOR = 264,
    BUZZER_ADDRESS = 0x08000070,

    MAPPING_BYTE_LENGTH = 1,
};

static int __init buzzer_init(void);
static void __exit buzzer_exit(void);
static int buzzer_open(struct inode* minode, struct file* mfile);
static int buzzer_release(struct inode* minode, struct file* mfile);
static ssize_t buzzer_read(struct file* inode, char* gdata, size_t length, loff_t* off_what);
static ssize_t buzzer_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what);

static struct file_operations buzzer_fops =
{
    .owner = THIS_MODULE,
    .open = buzzer_open,
    .write = buzzer_write,
    .read = buzzer_read,
    .release = buzzer_release, 
};

#endif /* BUZZER_DRIVER_H */