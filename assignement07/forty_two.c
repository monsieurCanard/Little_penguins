#include <linux/module.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/errno.h>

// #include "forty_two.h"

MODULE_AUTHOR("Anthony Gabriel");
MODULE_DESCRIPTION("Debugfs with 3 files");
MODULE_LICENSE("GPL");

static struct dentry *file;
static struct dentry *dir;

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



static int id_register(struct dentry *dir) {
	file = debugfs_create_file("id", 0666, dir, NULL, &fops);
	if (!file)
		return -ENOMEM;

	return 0;
}

static void id_unregister(void) {
	debugfs_remove(file);
}

static int __init forty_two_init(void)
{
	pr_info("Mymodule init called\n");
	int register_ret = 0;

	dir = debugfs_create_dir("forty_two", NULL);
	if(!dir) {
			pr_info("Impossible de creer le repertoire\n");
			return -ENOMEM;
	}

	register_ret = id_register(dir);
	if (register_ret) {
			pr_info("Impossible d'enregistrer les fichiers\n");
			debugfs_remove(dir);
			return register_ret;
	}

	return 0;
}

static void __exit forty_two_exit(void) {
	id_unregister();
	debugfs_remove(dir);
	pr_info("DELETE MODULES\n");
}

module_init(forty_two_init);
module_exit(forty_two_exit);