obj-$(CONFIG_CFS)    := cfs.o

cfs-y	:= compression.o super.o

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) CONFIG_CFS=m modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) CONFIG_CFS=m clean
