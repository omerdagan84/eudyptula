/*
 *   task9.c - Task 9 eudyptula.
 *   create a loadable kernel module and makefile
 *   implement 'sysfs entries'
 *   1. create subdirectory 'eudyptula' under '/sys/kernel'
 *   2. create a file called 'id' mode 666 - same as task6
 *   3. add a file called 'jiffies'mode 444 - read return kernel jiffies
 *   4. add a file 'foo' mode 644 - write data upto a PAGE size,
 *			read return the value
 *
 *
 *		Author: Omer Dagan
 *		Date: 26.4.16
 *		mail: omer.dagan@tandemg.com
 *		based on: kobject_example.c 
*/

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

#define MAX_INPUT_SIZE 20
static struct kobject *eudyptula_kobj;
static char *foo_data;

DEFINE_MUTEX(fops_lock);

static ssize_t foo_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int ret;
	if (mutex_lock_interruptible(&fops_lock))
		return -EINTR;
	ret = sprintf(buf, "%s\n", foo_data);

	mutex_unlock(&fops_lock);
	return ret;
}

static ssize_t foo_write(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	if (mutex_lock_interruptible(&fops_lock))
		return -EINTR;

	if (count > PAGE_SIZE)
		return -EINVAL;
	strcpy(foo_data, buf);
	pr_info("copied from the user %s count=%lu", foo_data, count);

	mutex_unlock(&fops_lock);
	return count;

}

static struct kobj_attribute foo_attribute =
	__ATTR(foo, 0644, foo_read, foo_write);

static ssize_t id_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "eudyptula\n");
}

static ssize_t id_write(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	char input[MAX_INPUT_SIZE];

	if (count > MAX_INPUT_SIZE)
		return -EINVAL;
	strcpy(input, buf);
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

static struct kobj_attribute id_attribute =
	__ATTR(id, 0664, id_read, id_write);

static ssize_t jiffies_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", jiffies);
}

static struct kobj_attribute jiffies_attribute =
	__ATTR(jiffies, 0444, jiffies_read, NULL);

static struct attribute *attrs[] = {
	&foo_attribute.attr,
	&id_attribute.attr,
	&jiffies_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init task8_init(void)
{
	int retval;

	eudyptula_kobj = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!eudyptula_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(eudyptula_kobj, &attr_group);
	if (retval)
		kobject_put(eudyptula_kobj);

	foo_data = (char *)kmalloc(PAGE_SIZE, GFP_KERNEL);

	pr_info("initialising task8 module - success");
	return retval;
}

static void __exit task8_exit(void)
{
	pr_debug("De-registered eudyptula debugfs module\n");
	kfree(foo_data);
	kobject_put(eudyptula_kobj);
}

MODULE_LICENSE("GPL"); /*cannot load debugfs without this*/
module_init(task8_init);
module_exit(task8_exit);
