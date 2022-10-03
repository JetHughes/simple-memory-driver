MODULE_NAME = memdrv

obj-m	:= $(MODULE_NAME).o

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

CFLAGS := -W -Wall -pedantic -g

all: module store retrieve-prog debug

store:
	gcc -w -Wall -pedantic store.c -o store-prog

retrieve-prog: retrieve.c libmemdrv.c libmemdrv.h
	gcc -O2 -w -Wall -o retrieve-prog retrieve.c libmemdrv.c 

test: 
	./store-prog testdata.txt

ins:
	sudo insmod memdrv.ko
	sudo chown mal:mal /dev/memdrv
	lsmod | grep memdrv
	ls -l /dev/memdrv

rm:
	sudo rmmod memdrv

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

debug: debug.c libmemdrv.c libmemdrv.h
	gcc $(CFLAGS) -o debug debug.c libmemdrv.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f debug
	rm -f store-prog
