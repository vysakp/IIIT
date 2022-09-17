#include <linux/kernel.h>

asmlinkage long vyshak_hello(void)
{
        printk("Hello .......!\n");
        return 0;
}