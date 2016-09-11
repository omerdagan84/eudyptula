/*
 *   task6.c - Task 6 eudyptula.
 *   create a loadable kernel module and makefile
 *   module will open a misc device /dev/eudyptula
 *
 *		Author: Omer Dagan
 *		Date: 21.4.16
 *		mail: omer.dagan@tandemg.com
 *		based on: http://linux.die.net/lkmpg/x121.html
*/

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define MAX_INPUT_SIZE 20

static ssize_t task17_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char input[MAX_INPUT_SIZE];
	int ret;

	if (count > MAX_INPUT_SIZE)
		return -EINVAL;
	ret = copy_from_user(input, buf, count);
	pr_info("copied from the user %s count=%lu", input, count);
/*	input[strcspn(input, "\r\n")] = '\0';
	if (strncmp(input, "acef8c84aaa6\0", count)) {
		pr_info("entered wrong input");
		return -EINVAL;
	} else {
		pr_debug("input acceptable");
	}*/
	return count + 1;

}

static const struct file_operations task17_fops = {
	.owner	= THIS_MODULE,
	.write	= task17_write,
};

static struct miscdevice eudyptula_dev = {
	MISC_DYNAMIC_MINOR,
	"eudyptula",
	&task17_fops
};

static int __init task17_init(void)
{
	int ret;

	ret = misc_register(&eudyptula_dev);
	if (ret)
		pr_debug("Unable to register eudyptula misc device\n");
	else
		pr_debug("Registered eudyptula misc device\n");
	return ret;
}

static void __exit task17_exit(void)
{
	pr_debug("De-registered eudyptula misc device\n");
	misc_deregister(&eudyptula_dev);
}

module_init(task17_init);
module_exit(task17_exit);
