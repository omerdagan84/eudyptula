#################################################################
#	eudyptula - task12 - Makefile								#
#																#
#	Author: Omer Dagan											#
#	Date: 9.5.16												#
#	Based on: http://www.tldp.org/LDP/lkmpg/2.6/html/x181.html	#											
#################################################################


obj-m += task12.o
CFLAGS_task9.o := -DDEBUG
all:
		make  -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
