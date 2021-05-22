#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int module_begin(void)
{
    printk(KERN_ALERT "hello, welcome to module!\n");
    return 0;
}

static void module_end(void)
{
    printk(KERN_ALERT "Goodbye, exit module\n");
}

module_init(module_begin);
module_exit(module_end);
