obj-m = push_switch_driver.o

PWD = $(shell pwd)

all: driver app

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

app:
	gcc -W -Wall -pedantic-errors -static -o test_push_switch test_push_switch.c

clean:
	rm -rf *.ko *.mod.* *.o test_led Module.symvers modules.order .led* .tmp* 

