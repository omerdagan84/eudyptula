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
#include <linux/ip.h>             
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/netdevice.h>      
#include <linux/netfilter.h>
#include <uapi/linux/netfilter_ipv4.h> 
#include <linux/skbuff.h>         
#include <linux/udp.h>                

/* ops struct */
static struct nf_hook_ops netfilter_ops;                        
static char *interface = "lo";                          

struct sk_buff *sock_buff;                              
struct udphdr *udp_header;                              
unsigned int main_hook(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*))
{
	unsigned char *user_data;
	unsigned char *it;
	unsigned char *tail;
	printk(KERN_EMERG "XXXX MAIN HOOK recived from interface %s XXXX",
			in->name);

	if(strcmp(in->name,interface) == 0){
		printk (KERN_EMERG "XXX Dropping by interface");
	   	return NF_DROP;
	}

	sock_buff = skb;
    if(!sock_buff) {
		printk (KERN_EMERG "XXX accepting");
		return NF_ACCEPT;
	}
			                  
	user_data = (unsigned char *)(sock_buff->data + (ip_hdr(sock_buff)->ihl *4)); 
	tail = skb_tail_pointer(skb);

	pr_debug("print - data:\n");
    for (it = user_data; it != tail; ++it) {
		char c = *(char *)it;
		if (c == '\0')
			break;
		if (c == 'd' && (*(char *)(it + 1)) == 'a')
		printk("%c", c);
	}
	printk("\n\n");

	return NF_ACCEPT;
}

static int __init task19_init(void)
{
	printk(KERN_EMERG "XXXX INIT XXXX");
	netfilter_ops.hook = (nf_hookfn *)main_hook;
	netfilter_ops.pf = PF_INET;        
	netfilter_ops.hooknum = NF_INET_PRE_ROUTING;
	netfilter_ops.priority = NF_IP_PRI_FIRST;
	nf_register_hook(&netfilter_ops);
	printk(KERN_EMERG "XXXX INIT XXXX");
											        
	return 0;
}

static void __exit task19_exit(void) 
{ 
	printk(KERN_EMERG "XXXX EXIT XXXX");
	nf_unregister_hook(&netfilter_ops); 
	printk(KERN_EMERG "XXXX EXIT XXXX");
}

module_init(task19_init);
module_exit(task19_exit);
