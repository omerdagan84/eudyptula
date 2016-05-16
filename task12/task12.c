/*
 *   task12.c - Task 12 eudyptula.
 *   create a loadable kernel module and makefile
 *
 *		Author: Omer Dagan
 *		Date: 9.5.16
 *		mail: omer.dagan@tandemg.com
 *		based on: http://linux.die.net/lkmpg/x121.html
 */

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/slab.h>

const char *cache_name = "eud";

struct kmem_cache *eud_cache;

struct identity {
	char  name[20];
	int   id;
	bool  busy;

	struct list_head list;
};

struct identity list_main;

struct identity *identity_find(int id)
{
	struct identity *tmp;

	list_for_each_entry(tmp, &list_main.list, list) {
		if (tmp->id == id)
			return tmp;
	}
	return NULL;
}

int identity_create(char *name, int id)
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

void identity_destroy(int id)
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

int __init initm(void)
{
	struct identity *temp;
	int err;

	pr_info("module loading");

	eud_cache = KMEM_CACHE(identity, SLAB_POISON);
	if (!eud_cache)
		return -ENOMEM;

	INIT_LIST_HEAD(&list_main.list);

	err = identity_create("Alice", 1);
	if (err)
		goto error;
	err = identity_create("Bob", 2);
	if (err)
		goto error;
	err = identity_create("Dave", 3);
	if (err)
		goto error;
	err = identity_create("Gena", 10);
	if (err)
		goto error;

	temp = identity_find(3);
	pr_info("id 3 = %s\n", temp->name);
	temp = identity_find(42);
	if (temp == NULL)
		pr_info("id 42 not found\n");

	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);

	return 0;
error:
	return -1;
}

void __exit removem(void)
{
	struct identity *temp;
	struct list_head *pos, *q;

	pr_info("unloading module\n");
	list_for_each_safe(pos, q, &list_main.list) {
		temp = list_entry(pos, struct identity, list);
		list_del(pos);
		kmem_cache_free(eud_cache, temp);
	}
	if (eud_cache)
		kmem_cache_destroy(eud_cache);
	pr_info("cache freed exiting\n");
}

module_init(initm);
module_exit(removem);
