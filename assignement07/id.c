#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/debugfs.h>

#include "debugfs.h"

MODULE_AUTHOR("Anthony Gabriel");
MODULE_DESCRIPTION("Login verification file");
MODULE_LICENSE("GPL");

#define BUF_SIZE 128
#define LOGIN "antgabri"
#define LOGIN_LEN 9

static ssize_t my_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos) {

	if (count > LOGIN_LEN - *ppos)
	       	count = LOGIN_LEN - *ppos;

	if (copy_to_user(user_buf, LOGIN + *ppos, count))
		return -EFAULT;

	*ppos += count;
	return count;
}

static ssize_t my_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos) {
	
	printk(KERN_INFO "my_write called with %zu", count);
	char buffer[BUF_SIZE];
	
	if(count == 0)
		return 0;

	if(count >= BUF_SIZE)
		count = BUF_SIZE - 1;

	if (copy_from_user(buffer, user_buf, count))
		return -EFAULT;

	buffer[count] = '\0';

	if(buffer[count - 1] == '\n')
		buffer[count - 1] = '\0';
	
	if(strcmp(buffer, LOGIN) != 0)
		return -EINVAL;
	
	printk(KERN_INFO "Successful Student Login Enter");	
	
	return count;
}

static struct file_operations fops = {
	.write = &my_write,
	.read = &my_read,
};

static struct dentry *file;


int id_register(struct dentry *dir) {
	file = debugfs_create_file("id", 0666, dir, NULL, &fops);
	if (!file)
		return -ENOMEM;

	return 0;
}

void id_unregister(void) {
	debugfs_remove(file);
}