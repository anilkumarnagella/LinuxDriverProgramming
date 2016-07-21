obj-m += hello.o

KDIR := /usr/src/linux-headers-4.4.0-24-generic

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

