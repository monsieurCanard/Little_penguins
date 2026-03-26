#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/errno.h>

// Dont have a license, LOL
MODULE_LICENSE("LICENSE");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static DEFINE_MUTEX(useless_lock);

char str[PAGE_SIZE];
char tmp[PAGE_SIZE];
size_t len = 0;

static ssize_t myfd_read(struct file *fp, char __user *user, size_t size,
			 loff_t *offs);

static ssize_t myfd_write(struct file *fp, const char __user *user, size_t size,
			  loff_t *offs);

static struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write,
};

static struct miscdevice myfd_device = { 
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

static int __init myfd_init(void)
{
	int retval;
	retval = misc_register(&myfd_device);
	return retval;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs)
{
	mutex_lock(&useless_lock)

	size_t i = 0;
	size_t t = 0;

	if (len == 0) {
		mutex_unlock(&useless_lock);
		return 0;
	}

	for (t = len; t > 0; t--, i++) {
		tmp[i] = str[t - 1];
	}
	tmp[i] = 0x0;

	ssize_t ret = simple_read_from_buffer(user, size, offs, tmp, i);
	mutex_unlock(&useless_lock);

	return ret;
}

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size,
		   loff_t *offs)
{
	mutex_lock(&useless_lock) ssize_t res = 0;

	if (size > PAGE_SIZE)
		size = PAGE_SIZE;

	res = simple_write_to_buffer(str, PAGE_SIZE, offs, user, size);
	if (res < 0) {
		mutex_unlock(&useless_lock);
		return res;
	}

	if (*offs < PAGE_SIZE)
		str[*offs] = 0x0;
	else
		str[PAGE_SIZE - 1] = 0x0;

	len = res;
	mutex_unlock(&useless_lock);
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);