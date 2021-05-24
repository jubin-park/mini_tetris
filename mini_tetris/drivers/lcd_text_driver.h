#ifndef LCD_TEXT_DRIVER_H
#define LCD_TEXT_DRIVER_H

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

#define LCD_TEXT_NAME ("csemad_lcd_text")

enum {
    LCD_TEXT_MAJOR = 263,
    LCD_TEXT_ADDRESS = 0x08000090,

    MAPPING_BYTE_LENGTH = 32,
};

static int __init lcd_text_init(void);
static void __exit lcd_text_exit(void);
static int lcd_text_open(struct inode* minode, struct file* mfile);
static int lcd_text_release(struct inode* minode, struct file* mfile);
static ssize_t lcd_text_read(struct file* inode, char* gdata, size_t length, loff_t* off_what);
static ssize_t lcd_text_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what);

static struct file_operations lcd_text_fops =
{
    .owner = THIS_MODULE,
    .open = lcd_text_open,
    .write = lcd_text_write,
    .read = lcd_text_read,
    .release = lcd_text_release, 
};

#endif /* LCD_TEXT_DRIVER_H */