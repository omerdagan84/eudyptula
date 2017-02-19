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

static unsigned char *ip_address = "\xC0\xA8\x00\x01"; 

static char *interface = "lo";                          

unsigned char *port = "\x00\x17";                       

struct sk_buff *sock_buff;                              
struct udphdr *udp_header;                              
unsigned int main_hook(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*))
{

	if(strcmp(in->name,interface) == 0)
	   	return NF_DROP;

	sock_buff = *skb;
    if(!sock_buff)
		return NF_ACCEPT;
	if(!(ip_hdr(sock_buff)))
	   	return NF_ACCEPT; 
	if(ip_hdr(sock_buff)->saddr == *(unsigned int*)ip_address)
	   	return NF_DROP;
			                  
	if(ip_hdr(sock_buff)->protocol != 17)
		return NF_ACCEPT;

	udp_header = (struct udphdr *)(sock_buff->data + (ip_hdr(sock_buff)->ihl *4)); 
	if((udp_header->dest) == *(unsigned short*)port)
		return NF_DROP;

	return NF_ACCEPT;
}

static int __init task19_init(void)
{
	netfilter_ops.hook = (nf_hookfn *)main_hook;
	netfilter_ops.pf = PF_INET;        
	netfilter_ops.hooknum = NF_INET_PRE_ROUTING;
	netfilter_ops.priority = NF_IP_PRI_FIRST;
	nf_register_hook(&netfilter_ops);
											        
	return 0;
}

static void __exit task19_exit(void) 
{ 
	nf_unregister_hook(&netfilter_ops); 
}

module_init(task19_init);
module_exit(task19_exit);
