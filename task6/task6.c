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

static ssize_t task6_read (struct file *file, char __user *buf,size_t count, loff_t *ppos)
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
static ssize_t task6_write (struct file *file, const char __user *buf,size_t count, loff_t *ppos)
{
	char input[MAX_INPUT_SIZE];
	int ret;

	if (count > MAX_INPUT_SIZE)
			return -EINVAL;
	ret = copy_from_user(input, buf, count - 1);
	printk(KERN_EMERG "copied from the user %s count=%lu", input, count);
	if (strncmp(input, "omerdagan", count)) {
		printk(KERN_EMERG "entered wrong input");
		return -EINVAL;
	} else {
		printk(KERN_EMERG "input acceptable");
	}
	return count + 1;

}
static const struct file_operations task6_fops = {
	.owner	= THIS_MODULE,
	.read	= task6_read,
	.write	= task6_write,
};
static struct miscdevice eudyptula_dev = {
	MISC_DYNAMIC_MINOR,
	"eudyptula",
	&task6_fops
};
static int __init task6_init(void)
{
	int ret;
	ret = misc_register(&eudyptula_dev);
	if (ret)
		printk(KERN_ERR "Unable to register eudyptula misc device\n");
	else
		printk(KERN_INFO "Registered eudyptula misc device\n");
    return ret;
}

static void __exit task6_exit(void)
{
	printk(KERN_INFO "De-registered eudyptula misc device\n");
	misc_deregister(&eudyptula_dev);
}

module_init(task6_init);
module_exit(task6_exit);
