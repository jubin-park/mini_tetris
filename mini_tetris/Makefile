obj-m = $(DRIVER_DIR)/dot_matrix_driver.o $(DRIVER_DIR)/led_driver.o

DRIVER_DIR = drivers

PWD = $(shell pwd)

all: driver app

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	sudo rmmod dot_matrix_driver led_driver
	sudo insmod $(DRIVER_DIR)/dot_matrix_driver.ko sudo$(DRIVER_DIR)/led_driver.ko

app:
	gcc -std=c99 -W -Wall -pedantic -static -o app_game app_game.c

clean:
	rm -rf *.ko *.mod.* *.o app_game Module.symvers modules.order .led* .tmp* 
	rm -rf $(DRIVER_DIR)/*.ko $(DRIVER_DIR)/*.mod.* $(DRIVER_DIR)/*.o
