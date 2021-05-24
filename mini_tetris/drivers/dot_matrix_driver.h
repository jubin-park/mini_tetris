#ifndef DOT_MATRIX_DRIVER_H
#define DOT_MATRIX_DRIVER_H

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

#define DOT_MATRIX_NAME ("csemad_dot_matrix")

enum {
    DOT_MATRIX_MAJOR = 262,
    DOT_MATRIX_ADDRESS = 0x08000210,
    
    MAPPING_BYTE_LENGTH = 20,

    ROW_COUNT = 10,
    COLUMN_COUNT = 7,
};

static int dot_matrix_open(struct inode* minode, struct file* mfile);
static int dot_matrix_release(struct inode* minode, struct file* mfile);
static ssize_t dot_matrix_read(struct file* inode, char* gdata, size_t length, loff_t* off_what);
static ssize_t dot_matrix_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what);
static int __init dot_matrix_init(void);
static void __exit dot_matrix_exit(void);

static struct file_operations dot_matrix_fops =
{
    .owner = THIS_MODULE,
    .open = dot_matrix_open,
    .write = dot_matrix_write,
    .read = dot_matrix_read,
    .release = dot_matrix_release, 
};

#endif // DOT_MATRIX_DRIVER_H