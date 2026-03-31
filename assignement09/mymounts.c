#include <linux/seq_file.h>
#include <linux/module>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

MODULE_AUTHOR("Anthony Gabriel");
MODULE_DESCRIPTION("Proc FS for listing every mounted point");
MODULE_LICENSE("GPL");

static struct proc_dir_entry *proc_file;

// static ssize_t 


const struct proc_ops mymount_fops = {
	.owner = THIS_MODULE,
	.open = &mymount_open,
	.read = &mymount_read,
};



static int my_module_init(void) {

	int ret;
	ret = proc_create_data("mymount", 0644, NULL, &fops);
	if(!ret)
	{
		pr_error("Proc module cannot be initialized !\n");
		return -ENOMEM;
	}
	pr_info("Proc module mymount init DONE\n");
	return ret;
}

static void __exit module_exit(void) {
	proc_remove_data()
}

module_init(my_module_init);
