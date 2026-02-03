#include <linux/init.h>
#include <linux/module.h>

MODULE_AUTHOR("Anthony Gabriel");
MODULE_DESCRIPTION("Hello World 42Paris exercice");
MODULE_LICENSE("GPL");

static int __init custom_init(void) {
	printk(KERN_INFO "Hello world!\n");
	return 0;
}

static void __exit custom_exit(void) {
	printk(KERN_INFO "Cleaning up module.\n");
	return;
}

module_init(custom_init);
module_exit(custom_exit);