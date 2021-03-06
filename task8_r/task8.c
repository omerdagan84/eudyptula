/*
 *   task6.c - Task 8 kerneltask.
 *   create a loadable kernel module and makefile
 *   implement 'debugfs entries'
 *   1. create subdirectory 'kerneltask'
 *   2. create a file called 'id' mode 666 - same as task6
 *   3. add a file called 'jiffies'mode 444 - read return kernel jiffies
 *   4. add a file 'foo' mode 644 - write data upto a PAGE size,
 *			read return the value
 *
 *
 *		Author: Omer Dagan
 *		Date: 25.4.16
 *		mail: omer.dagan@tandemg.com
 *		based on: http://linux.die.net/lkmpg/x121.html
*/

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#define MAX_INPUT_SIZE 20
static struct dentry *debug_dir;
static char *foo_data;

DEFINE_MUTEX(fops_lock);

static ssize_t foo_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	if (mutex_lock_interruptible(&fops_lock))
		return -EINTR;

	if (*ppos == strlen(foo_data)) {
		mutex_unlock(&fops_lock);
		return 0;
	} else
		if (*ppos != 0 || count < strlen(foo_data))
			goto err;

	if (copy_to_user(buf, foo_data, strlen(foo_data)))
		goto err;

	*ppos = strlen(foo_data);
	mutex_unlock(&fops_lock);
	return *ppos;
err:
	mutex_unlock(&fops_lock);
	return -EINVAL;
}

static ssize_t foo_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int ret;

	if (mutex_lock_interruptible(&fops_lock))
		return -EINTR;

	if (count > PAGE_SIZE)
		return -EINVAL;
	ret = copy_from_user(foo_data, buf, count);
	pr_info("copied from the user %s count=%lu", foo_data, count);

	mutex_unlock(&fops_lock);
	return count;

}
static const struct file_operations foo_fops = {
	.owner	= THIS_MODULE,
	.read	= foo_read,
	.write	= foo_write,
};

static ssize_t id_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{

	char ret_string[] = "kerneltask\n";

	if (*ppos == sizeof(ret_string))
		return 0;
	else
		if (*ppos != 0 || count < sizeof(ret_string))
			return -EINVAL;

	if (copy_to_user(buf, ret_string, sizeof(ret_string)))
		return -EINVAL;
	*ppos = sizeof(ret_string);
	return *ppos;
}

static ssize_t id_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char input[MAX_INPUT_SIZE];
	int ret;

	if (count > MAX_INPUT_SIZE)
		return -EINVAL;
	ret = copy_from_user(input, buf, count);
	pr_info("copied from the user %s count=%lu", input, count);
	input[strcspn(input, "\r\n")] = '\0';
	if (strncmp(input, "acef8c84aaa6\0", count)) {
		pr_info("entered wrong input");
		return -EINVAL;
	} else {
		pr_debug("input acceptable");
	}
	return count + 1;

}
static const struct file_operations id_fops = {
	.owner	= THIS_MODULE,
	.read	= id_read,
	.write	= id_write,
};


static ssize_t jiffies_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{

	char ret_string[25];
	int str_size;

	str_size = sprintf(ret_string, "%lu", jiffies);

	if (*ppos == str_size)
		return 0;
	else
		if (*ppos != 0 || count < str_size)
			return -EINVAL;

	if (copy_to_user(buf, ret_string, str_size))
		return -EINVAL;

	*ppos = str_size;
	return *ppos;
}

static const struct file_operations jiffies_fops = {
	.owner	= THIS_MODULE,
	.read	= jiffies_read,
	.write	= NULL,
};

static int __init task8_init(void)
{
	struct dentry *debug_entry = NULL;

	/*create the debugfs directory*/
	debug_dir = debugfs_create_dir("kerneltask", NULL);
	if (!debug_dir) {
		pr_info("could not create dir - exiting...");
		return -ENODEV;
	}

	debug_entry = debugfs_create_file("id", 0666, debug_dir, NULL, &id_fops);
	if (!debug_entry)
		goto cleanup_debugfs;

	debug_entry = debugfs_create_file("jiffies", 0444, debug_dir, NULL, &jiffies_fops);
	if (!debug_entry)
		goto cleanup_debugfs;

	foo_data = (char *)kmalloc(PAGE_SIZE, GFP_KERNEL);

	debug_entry = debugfs_create_file("foo", 0644, debug_dir, NULL, &foo_fops);
	if (!debug_entry)
		goto cleanup_mem;

	pr_info("initialising task8 module - success");
	return 0;
cleanup_mem:
	kfree(foo_data);
cleanup_debugfs:
	debugfs_remove_recursive(debug_dir);
	return -EINVAL;
}

static void __exit task8_exit(void)
{
	pr_debug("De-registered kerneltask debugfs module\n");
	kfree(foo_data);
	debugfs_remove_recursive(debug_dir);
}

MODULE_LICENSE("GPL"); /*cannot load debugfs without this*/
module_init(task8_init);
module_exit(task8_exit);
