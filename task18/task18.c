/*
 *   task18.c - Task 18 kerneltask.
 *   create a loadable kernel module and makefile
 *   module will open a misc device /dev/kerneltask
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
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/spinlock.h>

#define MAX_INPUT_SIZE 19

static int id_counter;
const char *cache_name = "eud";

struct kmem_cache *eud_cache;

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
	spinlock_t			lock;
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

	strcpy(temp->name, name);
	temp->id = id;
	temp->busy = false;

	pr_info("creating identity - name: '%s', id: '%d'\n", temp->name, temp->id);
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

static struct identity *identity_get(void)
{
	struct list_head *pos, *q;
	struct identity *tmp;

	if (list_empty(&list_main.list))
		return NULL;

	list_for_each_safe(pos, q, &list_main.list) {
		tmp = list_entry(pos, struct identity, list);
		list_del(pos);
		break;
	}
	pr_info("%s : got entry - name: '%s' id: '%d'\n", __func__, tmp->name, tmp->id);
	return tmp;
}

static ssize_t task18_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char input[MAX_INPUT_SIZE];
	int ret;

	if (count > MAX_INPUT_SIZE)
		ret = copy_from_user(input, buf, MAX_INPUT_SIZE);
	else
		ret = copy_from_user(input, buf, count - 1);

	spin_lock(&task_ctx.lock);
	ret = identity_create(input, id_counter++);

	task_ctx.f_wait++;
	spin_unlock(&task_ctx.lock);
	wake_up_all(&task_ctx.wee_wait);

	pr_info("copied from the user %s count=%lu and woke-up the thread\n", input, count);
	return count + 1;

}

static const struct file_operations task18_fops = {
	.owner	= THIS_MODULE,
	.write	= task18_write,
};

static struct miscdevice kerneltask_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "kerneltask",
	.fops = &task18_fops,
	.mode = S_IWUGO
};

static int run_thread(void *arg)
{
	int rc;
	struct identity *id_to_display = NULL;
wait_on:
	pr_debug("thread - starting wait on \"wee-wait\"\n");
	rc = wait_event_killable(task_ctx.wee_wait, task_ctx.f_wait);
	/* check if the wait was interrupted */
	if (rc == -ERESTARTSYS)
		return rc;
	while (task_ctx.f_wait) {
		spin_lock(&task_ctx.lock);
		id_to_display = identity_get();
		if (!id_to_display)
			return -1;
		msleep_interruptible(5 * 1000);
		pr_debug("thread finished sleep\n");
		pr_emerg("IDENTITY OUTPUT :\n   name:'%s'\n   id:'%d'", id_to_display->name, id_to_display->id);
		kmem_cache_free(eud_cache, id_to_display);
		task_ctx.f_wait--;
		spin_unlock(&task_ctx.lock);
	}
	goto wait_on;
	return 0;
}

static int __init task18_init(void)
{
	int ret;

	/* Initialize the wait queue */
	init_waitqueue_head(&task_ctx.wee_wait);
	INIT_LIST_HEAD(&list_main.list);
	spin_lock_init(&task_ctx.lock);

	eud_cache = KMEM_CACHE(identity, SLAB_POISON);
	if (!eud_cache)
		return -ENOMEM;

	/* Setup the thread */
	task_ctx.f_wait = 0; /*condition for wait on queue*/
	task_ctx.task_thread = kthread_run(run_thread, NULL, "kerneltask"); /*thread is stopped*/
	if (IS_ERR(task_ctx.task_thread)) {
		ret = PTR_ERR(task_ctx.task_thread);
		return ret;
	}

	id_counter = 0;
	ret = misc_register(&kerneltask_dev); /* register the misc device*/
	if (ret) {
		pr_debug("Unable to register kerneltask misc device\n");
		return ret;
	} else {
		pr_debug("Registered kerneltask misc device\n");
	}

	return ret;
}

static void __exit task18_exit(void)
{
	struct identity *temp;
	struct list_head *pos, *q;

	pr_debug("De-registered kerneltask misc device\n");
	misc_deregister(&kerneltask_dev);

	pr_debug("waking-up thread for termination execution\n");
	spin_lock(&task_ctx.lock);
	task_ctx.f_wait = 1;
	spin_unlock(&task_ctx.lock);
	wake_up_all(&task_ctx.wee_wait);

	pr_info("clearing the identity list\n");
	list_for_each_safe(pos, q, &list_main.list) {
		temp = list_entry(pos, struct identity, list);
		pr_info("list cleaning: temp->name '%s' temp->id '%d'\n", temp->name, temp->id);
		list_del(pos);
		kmem_cache_free(eud_cache, temp);
	}
	kmem_cache_destroy(eud_cache);

	pr_info("module removed!!\n");
}

module_init(task18_init);
module_exit(task18_exit);
