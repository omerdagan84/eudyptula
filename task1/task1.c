/********************************************************	  
 *   task1.c - Task 1 eudyptula.						*
 *   create a loadable kernel module and makefile		*
 *														*	
 *		Author: Omer Dagan								*
 *		Date: 11.4.16									*
 *		mail: omer.dagan@tandemg.com					*
 *		based on: http://linux.die.net/lkmpg/x121.html	*
 ********************************************************/

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

int __init initm(void)
{
		printk(KERN_INFO "Hello World!\n");

		return 0;
}

void __exit removem(void)
{
		printk(KERN_INFO "unloading module\n");
}

module_init(initm);
module_exit(removem);
