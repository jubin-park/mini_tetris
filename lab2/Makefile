obj-m = led_driver.o

PWD = $(shell pwd)

all: driver app

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

app:
	gcc -Wall -static -o test_led test_led.c

clean:
	rm -rf *.ko *.mod.* *.o test_led Module.symvers modules.order .led* .tmp* 

