MODULE_NAME = memdrv

obj-m	:= $(MODULE_NAME).o

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

CFLAGS := -W -Wall -pedantic -g

all: module store-prog retrieve-prog debug

store-prog:
	gcc -w -Wall -o store-prog store.c

retrieve-prog:
	gcc -w -Wall -o retrieve-prog retrieve.c libmemdrv.c 

install:
	sudo insmod memdrv.ko
	sudo chown mal:mal /dev/memdrv
	lsmod | grep memdrv
	ls -l /dev/memdrv

remove:
	sudo rmmod memdrv

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

debug: debug.c libmemdrv.c libmemdrv.h
	gcc $(CFLAGS) -o debug debug.c libmemdrv.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f debug
	rm -f store-prog
	rm -f retrieve-prog
