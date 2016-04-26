/*
 *   task8.c - Task 8 eudyptula.
 *   create a loadable kernel module and makefile
 *   implement 'debugfs entries'
 *   1. create subdirectory 'eudyptula'
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
#include <linux/spinlock.h>
#include <linux/slab.h>

#define MAX_INPUT_SIZE 20
static struct dentry *debug_dir;

static ssize_t id_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{

	char ret_string[] = "eudyptula\n";

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

static int __init task8_init(void)
{
	struct dentry *debug_entry = NULL;

	/*create the debugfs directory*/
	debug_dir = debugfs_create_dir("eudyptula", NULL);
	if (!debug_dir) {
		pr_info("could not create dir - exiting...");
		return -ENODEV;
	}

	debug_entry = debugfs_create_file("id", 0666, debug_dir, NULL, &id_fops);
	if (!debug_entry)
		goto cleanup_debugfs;

	pr_info("initialising task8 module - success");
	return 0;
cleanup_debugfs:
	return -EINVAL;
}

static void __exit task8_exit(void)
{
	pr_debug("De-registered eudyptula debugfs module\n");
	debugfs_remove_recursive(debug_dir);
}

MODULE_LICENSE("GPL"); /*cannot load debugfs without this*/
module_init(task8_init);
module_exit(task8_exit);
