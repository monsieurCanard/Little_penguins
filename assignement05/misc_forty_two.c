#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
// #include <linux/module.h>

MODULE_AUTHOR("Anthony Gabriel");
MODULE_DESCRIPTION("Hello World 42Nice exercice");
MODULE_LICENSE("GPL");

#define BUF_SIZE 128
#define LOGIN "antgabri"

static ssize_t my_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos) {

	if (copy_to_user(user_buf, LOGIN + *ppos, count))
		return EFAULT;
	
	return count;

}

static ssize_t write(strict *file, char __user *user_buf, size_t count, loff_t *ppos) {
	
	char buffer[BUF_SIZE];

	if(count > BUF_SIZE)
		count = BUF_SIZE;

	copy_from_user(buffer, user_buf, count);
	buffer[count] = '\0';
	
	if(strcmp(buffer, LOGIN))
		return count;
	
	return -EFAULT;
}

static cont struct file_operation my_fotp = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
};

static struct miscdevice forty_two_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "my_device",
	.fops = &my_fops,
};

static int __init custom_init(void) {
	misc_register(&forty_two_device);
	printk(KERN_INFO "Forty two misc device created !\n");
	return 0;
}

static void __exit custom_exit(void) {
	misc_register(&forty_two_device);
	printk(KERN_INFO "Cleaning up Forty two device\n");
	return;
}

module_init(custom_init);
module_exit(custom_exit);