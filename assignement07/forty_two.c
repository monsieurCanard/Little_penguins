#include <linux/module.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/errno.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>

MODULE_AUTHOR("Anthony Gabriel");
MODULE_DESCRIPTION("Debugfs with 3 files. ID - write(verify login) / read(write login) JIFFIES - read(give you jiffies time values) FOO - write ONLY ROOT (write data in file) / read(get content of file)");
MODULE_LICENSE("GPL");

#define BUF_SIZE 128
#define LOGIN "antgabri"
#define LOGIN_LEN 9

static struct dentry *file_id = NULL, *file_jiffies = NULL, *file_foo = NULL;
static struct dentry *dir = NULL;
static char data_foo_file[PAGE_SIZE];
static ssize_t data_foo_available = 0;

static DEFINE_MUTEX(foo_lock);

int error(char* reason, int return_value);

//FOO FILE
static ssize_t foo_write(struct file *file,const char __user *user_buf, size_t count, loff_t *pos)
{
	mutex_lock(&foo_lock);

	if (*pos >= PAGE_SIZE) {
		mutex_unlock(&foo_lock);
		return 0;
	}

	*pos = data_foo_available;
	if(count > PAGE_SIZE - *pos)
		count = PAGE_SIZE - *pos;

	if(copy_from_user(data_foo_file + *pos, user_buf, count)) {
		mutex_unlock(&foo_lock);
		return -EFAULT;
	}

	*pos += count;
	data_foo_available = *pos;
	mutex_unlock(&foo_lock);
	return count;
}


static ssize_t foo_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos)
{
	mutex_lock(&foo_lock);
	
	if(*pos >= data_foo_available) {
		mutex_unlock(&foo_lock);
		return 0;
	}

	if (count > data_foo_available - *pos)
		count = data_foo_available - *pos;
	
	if(copy_to_user(user_buf, data_foo_file + *pos, count)) {
		mutex_unlock(&foo_lock);
		return -EFAULT;
	}
	
	*pos += count;
	mutex_unlock(&foo_lock);
	return count;
}

static struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.write = &foo_write,
	.read = &foo_read,
};

static int foo_register(void) {
	file_foo = debugfs_create_file("foo", 0644, dir, NULL, &foo_fops);
	if (!file_foo)
		return -ENOMEM;

	return 0;
}

static void foo_unregister(void) {
	debugfs_remove(file_foo);
}

//JIFFIES FILE
static ssize_t jiffies_read(struct file *file_jiffies, char __user *user_buff, size_t count, loff_t *pos)
{
	if (*pos > 0)
		return 0;
	
	char buf_time[32];
	int len = 0;

	len = snprintf(buf_time, sizeof(buf_time), "%lu\n", jiffies);

	if (copy_to_user(user_buff, buf_time, len))
		return -EFAULT;
	
	*pos = len;
	return len;
}

static struct file_operations fops_jiffies =
{
	.owner = THIS_MODULE,
	.read = &jiffies_read,
};

static int jiffies_register(void) {
	file_jiffies = debugfs_create_file("jiffies", 0444, dir, NULL, &fops_jiffies);
	if (!file_jiffies)
		return -ENOMEM;

	return 0;
}

static void jiffies_unregister(void)
{
	debugfs_remove(file_jiffies);
}

//ID FILE
static ssize_t id_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos) {

	if (count > LOGIN_LEN - *ppos)
		count = LOGIN_LEN - *ppos;

	if (copy_to_user(user_buf, LOGIN + *ppos, count))
		return -EFAULT;

	*ppos += count;
	return count;
}

static ssize_t id_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos) {
	
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

static struct file_operations fops_id = {
	.owner = THIS_MODULE,
	.write = &id_write,
	.read = &id_read,
};

static int id_register(void) {
	file_id = debugfs_create_file("id", 0666, dir, NULL, &fops_id);
	if (!file_id)
		return -ENOMEM;

	return 0;
}

static void id_unregister(void) {
	debugfs_remove(file_id);
}

/// PRINCIPAL MODULE
static int __init forty_two_init(void)
{
	pr_info("Mymodule init called\n");
	int register_ret = 0;

	dir = debugfs_create_dir("forty_two", NULL);
	if(!dir)
		return error("debugfs directory", -ENOMEM);

	register_ret = id_register();
	if (register_ret)
		return error("id file", register_ret);

	register_ret = jiffies_register();
	if (register_ret)
		return error("file jiffies register", register_ret);

	register_ret = foo_register();
	if(register_ret)
		return error("file foo register", register_ret);

	return 0;
}

static void __exit forty_two_exit(void) {
	id_unregister();
	jiffies_unregister();
	foo_unregister();
	debugfs_remove(dir);
	pr_info("DELETE MODULES\n");
}

int error(char* reason, int return_value) {
	pr_info("Issue with %s, abording debugfs creation\n", reason);
	
	debugfs_remove(dir);
	id_unregister();
	jiffies_unregister();
	foo_unregister();
	return return_value;
}

module_init(forty_two_init);
module_exit(forty_two_exit);