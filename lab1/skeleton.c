#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/major.h>

MODULE_LICENSE("GPL");

static int g_result;
static const int MAJOR_VERSION = 248;
static const char* KERNEL_NAME = "skeleton";

static int skeleton_open(struct inode *inode, struct file *filp) 
{
	printk(KERN_ALERT "Device Open!!\n");
	return 0;
}

static int skeleton_release(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "Device Release!!\n");
	return 0;
}

static int skeleton_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	printk("Device Read\n");
	return 0;
}

static long skeleton_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("Device Ioctl!!\n");
	return 0;
}

static ssize_t skeleton_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	printk("Device Write!!\n");
	return 0;
}

static struct file_operations skeleton_fops = {
	.open = skeleton_open,
	.release = skeleton_release,
	.read = skeleton_read,
	.unlocked_ioctl = skeleton_ioctl,
	.write = skeleton_write,
};

static int __init skeleton_init(void)
{
	printk(KERN_ALERT "skeleton module init!!\n");
	g_result = register_chrdev(MAJOR_VERSION, KERNEL_NAME, &skeleton_fops);
	printk("return value=%d\n", g_result);

	return 0;
}

static void __exit skeleton_exit(void)
{
	unregister_chrdev(MAJOR_VERSION, KERNEL_NAME);
	printk(KERN_ALERT "skeleton module exit!!\n");
}

module_init(skeleton_init);
module_exit(skeleton_exit);
