/*
 *   task18.c - Task 18 eudyptula.
 *   create a loadable kernel module and makefile
 *   module will open a misc device /dev/eudyptula
 *
 *		Author: Omer Dagan
 *		Date: 02.01.17
 *		mail: omerdagan84@gmail.com
 *		based on: http://linux.die.net/lkmpg/x121.html
*/

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/kthread.h>

#define MAX_INPUT_SIZE 19

struct identity {
	char  name[20];
	int   id;
	bool  busy;

	struct list_head list;
};

struct task_context {
	struct task_struct *task_thread;
	wait_queue_head_t	wee_wait;
	int					f_wait;
};
static struct task_context task_ctx;

static struct identity list_main;

static struct identity *identity_find(int id)
{
	struct identity *tmp;

	list_for_each_entry(tmp, &list_main.list, list) {
		if (tmp->id == id)
			return tmp;
	}
	return NULL;
}

static int identity_create(char *name, int id)
{
	struct identity *temp;

	temp = kmem_cache_alloc(eud_cache, GFP_KERNEL);
	if (!temp)
		return -ENOMEM;

	pr_info("unloading module\n");
	strcpy(temp->name, name);
	temp->id = id;
	temp->busy = false;

	list_add(&(temp->list), &(list_main.list));
	return 0;
}

static void identity_destroy(int id)
{
	struct list_head *pos, *q;
	struct identity *tmp;

	list_for_each_safe(pos, q, &list_main.list) {
		tmp = list_entry(pos, struct identity, list);
			if (tmp->id == id) {
				list_del(pos);
				kmem_cache_free(eud_cache, tmp);
				return;
		}
	}
}

static ssize_t task17_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char input[MAX_INPUT_SIZE];
	int ret;

	if (count > MAX_INPUT_SIZE)
		return -EINVAL;
	ret = copy_from_user(input, buf, count);
	pr_info("copied from the user %s count=%lu", input, count);
	return count + 1;

}

static const struct file_operations task17_fops = {
	.owner	= THIS_MODULE,
	.write	= task17_write,
};

static struct miscdevice eudyptula_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &task17_fops,
	.mode = S_IWUGO
};

static int run_thread(void *arg)
{
	int rc;

	pr_debug("thread - starting wait on \"wee-wait\"\n");
	wait_event_killable(task_ctx.wee_wait, task_ctx.f_wait);
	pr_debug("thread - done wait on \"wee-wait\"\n");
	return 0;
}

static int __init task17_init(void)
{
	int ret;

	/* Initialize the wait queue */
	init_waitqueue_head(&task_ctx.wee_wait);
	INIT_LIST_HEAD(&list_main.list);

	/* Setup the thread */
	task_ctx.f_wait = 0; /*condition for wait on queue*/
	task_ctx.task_thread = kthread_run(run_thread, NULL, "eudyptula"); /*thread is stopped*/
	if (IS_ERR(task_ctx.task_thread)) {
		ret = PTR_ERR(task_ctx.task_thread);
		return ret;
	}

	ret = misc_register(&eudyptula_dev); /* register the misc device*/
	if (ret) {
		pr_debug("Unable to register eudyptula misc device\n");
		return ret;
	} else {
		pr_debug("Registered eudyptula misc device\n");
	}

	return ret;
}

static void __exit task17_exit(void)
{
	pr_debug("De-registered eudyptula misc device\n");
	task_ctx.f_wait = 1;
	wake_up_all(&task_ctx.wee_wait);

	misc_deregister(&eudyptula_dev);
}

module_init(task17_init);
module_exit(task17_exit);
