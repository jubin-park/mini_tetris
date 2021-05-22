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

#define CSEMAD_DOT_MATRIX_NAME "csemad_dot_matrix"
enum {
    CSEMAD_PUSH_SWITCH_MAJOR = 262,
    CSEMAD_PUSH_SWITCH_ADDRESS = 0x08000210,
    
    MAPPING_BYTE_LENGTH = 20,

    ROW_COUNT = 10,
    COLUMN_COUNT = 7,
};

static int csemad_dot_matrix_open(struct inode* minode, struct file* mfile);
static int csemad_dot_matrix_release(struct inode* minode, struct file* mfile);
static ssize_t csemad_dot_matrix_read(struct file* inode, char* gdata, size_t length, loff_t* off_what);
static ssize_t csemad_dot_matrix_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what);
static int __init csemad_dot_matrix_init(void);
static void __exit csemad_dot_matrix_exit(void);

static struct file_operations csemad_dot_matrix_fops =
{
    .owner = THIS_MODULE,
    .open = csemad_dot_matrix_open,
    .write = csemad_dot_matrix_write,
    .read = csemad_dot_matrix_read,
    .release = csemad_dot_matrix_release, 
};

#endif // DOT_MATRIX_DRIVER_H