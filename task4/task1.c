/*	  
 *   task1.c - Task 1 eudyptula.						
 *   create a loadable kernel module and makefile		
 *															
 *		Author: Omer Dagan								
 *		Date: 11.4.16									
 *		mail: omer.dagan@tandemg.com					
 *		based on: http://linux.die.net/lkmpg/x121.html	
 */

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/printk.h>

int __init initm(void)
{
		pr_info("Hello World!\n");

		return 0;
}

void __exit removem(void)
{
		pr_info("unloading module\n");
}

module_init(initm);
module_exit(removem);
