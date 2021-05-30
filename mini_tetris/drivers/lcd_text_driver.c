#include "lcd_text_driver.h"

static unsigned char s_usage;
static unsigned char* s_lcd_text_addr;

static int __init lcd_text_init(void)
{
    int chdev;
    printk(KERN_ALERT "lcd_text_init!\n");

    chdev = register_chrdev(LCD_TEXT_MAJOR, LCD_TEXT_NAME, &lcd_text_fops);
    
    if (0 != chdev) {
        printk(KERN_ALERT "The major number (%d) is busy\n", chdev);
        return 1;
    }

    s_lcd_text_addr = (unsigned char*)ioremap(LCD_TEXT_ADDRESS, MAPPING_BYTE_LENGTH);
    return 0;
}

static void __exit lcd_text_exit(void)
{
    unregister_chrdev(LCD_TEXT_MAJOR, LCD_TEXT_NAME);
    iounmap(s_lcd_text_addr);

    printk(KERN_ALERT "lcd_text_exit!\n");
}

static int lcd_text_open(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "lcd_text_open!\n");

    if (s_usage != 0) {
        printk(KERN_ALERT "lcd_text_open failcd_text: -EBUSY\n");
        return -EBUSY;
    }

    s_usage = 1;

    return 0;
}

static int lcd_text_release(struct inode* minode, struct file* mfile)
{
    printk(KERN_ALERT "lcd_text_release!\n");
    s_usage = 0;

    return 0;
}

static ssize_t lcd_text_read(struct file* inode, char* gdata, size_t length, loff_t* off_what)
{
    printk(KERN_ALERT "lcd_text_read!\n");

    if (0 != copy_to_user(gdata, s_lcd_text_addr, MAPPING_BYTE_LENGTH)) {
        printk(KERN_ALERT "lcd_text_read failcd_text: -EFAULT\n");
        return -EFAULT;
    }

    printk(KERN_ALERT "s_lcd_text_addr:\t%p\t%d\n", s_lcd_text_addr, *s_lcd_text_addr);
    printk(KERN_ALERT "gdata:\t\t\t%p\t%d\n", gdata, *gdata);

    return length;
}

static ssize_t lcd_text_write(struct file* inode, const char* gdata, size_t length, loff_t* off_what)
{
    int i;
    unsigned short int _s_value = 0;
    unsigned char value[MAPPING_BYTE_LENGTH];
    const char *tmp = gdata;

    if (copy_from_user(&value, tmp, length)) {
        return -EFAULT;
    }
    
    value[length] = 0;
    
    for (i = 0; i < length; i++) {
        _s_value = (value[i] & 0xFF) << 8 | (value[i + 1] & 0xFF);

        printk(KERN_ALERT "lcd_text_write\t%x = %x | %x\n", _s_value, (value[i] & 0xFF), (value[i + 1] & 0xFF));

        outw(_s_value, (unsigned int)s_lcd_text_addr + i);
    }

    return length;
}

module_init(lcd_text_init);
module_exit(lcd_text_exit);