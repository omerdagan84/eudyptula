#################################################################
#	kerneltask - task17 - Makefile								#
#																#
#	Author: Omer Dagan											#
#	Date: 11.9.16												#
#	Based on: http://www.tldp.org/LDP/lkmpg/2.6/html/x181.html	#											
#################################################################


obj-m += task17.o
CFLAGS_task17.o := -DDEBUG
all:
		make  -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
