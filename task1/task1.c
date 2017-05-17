/*
 *   task5.c - Task 5 kerneltask.						
 *   create a loadable kernel module and makefile		
 *   module is loaded on USB-keyboard connect
 *															
 *		Author: Omer Dagan								
 *		Date: 12.4.16									
 *		mail: omer.dagan@tandemg.com					
 *		based on: http://linux.die.net/lkmpg/x121.html	
*/

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/usb.h>		/* use of USB operations*/


/*declare the USB DEVICE*/
static struct usb_device_id my_id_table[] = {
	{USB_DEVICE(0x045e,0x078c)},
	{}
};
/*register the id table*/
MODULE_DEVICE_TABLE(usb,my_id_table);

static int task5_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
		pr_info("task5 module is probing...");
		return 0;
}

static void task5_disconnect(struct usb_interface *interface)
{
		pr_info("task5 module in disconnect");
}

static struct usb_driver task5_driver = {
	.name =		"task5_driver",
	.probe =	task5_probe,
	.disconnect = task5_disconnect,
	.id_table = my_id_table,
};

static int __init task5_init(void)
{
		int retval = 0;
		pr_info("start task5 module init\n");
		retval = usb_register(&task5_driver);
		if (retval)
			pr_info("task5 module failed to register");
		
		return 0;
}

static void __exit task5_exit(void)
{
		usb_deregister(&task5_driver);
		pr_info("task5 module is unloaded\n");
}

module_init(task5_init);
module_exit(task5_exit);
