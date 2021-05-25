DRIVER_DIR = drivers
PWD = $(shell pwd)

obj-m = $(DRIVER_DIR)/dot_matrix_driver.o $(DRIVER_DIR)/led_driver.o $(DRIVER_DIR)/buzzer_driver.o $(DRIVER_DIR)/seven_segment_driver.o $(DRIVER_DIR)/push_switch_driver.o #$(DRIVER_DIR)/lcd_text_driver.o

all: driver app

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

app:
	gcc -std=c99 -W -Wall -pedantic -static -o app_game app_game.c

clean:
	rm -rf *.ko *.mod.* *.o app_game Module.symvers modules.order .led* .tmp* 
	rm -rf $(DRIVER_DIR)/*.ko $(DRIVER_DIR)/*.mod.* $(DRIVER_DIR)/*.o