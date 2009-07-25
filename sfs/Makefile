obj-m    := sfs.o

sfs-y	:= super.o

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
test:
	umount /mnt 2> /dev/null
	rmmod sfs 2> /dev/null
	insmod sfs.ko
	mount -t sfs none /mnt
