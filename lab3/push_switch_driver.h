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

#define CSEMAD_PUSH_SWITCH_NAME "csemad_push_switch"
enum {
    CSEMAD_PUSH_SWITCH_MAJOR = 265,
    CSEMAD_PUSH_SWITCH_ADDRESS = 0x08000050,
    MAPPING_BYTE_LENGTH = 9,
};

static int csemad_push_switch_open(struct inode* minode, struct file* mfile);
static int csemad_push_switch_release(struct inode* minode, struct file* mfile);
static ssize_t csemad_push_switch_read(struct file* inode, char* gdata, size_t length, loff_t* off_what);
static ssize_t csemad_push_switch_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what);
static int __init csemad_push_switch_init(void);
static void __exit csemad_push_switch_exit(void);

static struct file_operations csemad_push_switch_fops =
{
    .owner = THIS_MODULE,
    .open = csemad_push_switch_open,
    .write = csemad_push_switch_write,
    .read = csemad_push_switch_read,
    .release = csemad_push_switch_release, 
};

#endif // PUSH_SWITCH_DRIVER_H