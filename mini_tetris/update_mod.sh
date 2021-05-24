DRIVER_DIR="drivers"

echo "Update FPGA module drivers ..."

sudo mknod /dev/csemad_led c 260 0
sudo mknod /dev/csemad_seven_segment c 261 0
sudo mknod /dev/csemad_dot_matrix c 262 0
sudo mknod /dev/csemad_lcd_text c 263 0
sudo mknod /dev/csemad_buzzer c 264 0
sudo mknod /dev/csemad_push_switch c 265 0

sudo rmmod led_driver
sudo rmmod seven_segment_driver
sudo rmmod dot_matrix_driver
sudo rmmod lcd_text_driver
sudo rmmod buzzer_driver
sudo rmmod push_switch_driver

sudo insmod ./${DRIVER_DIR}/led_driver.ko
sudo insmod ./${DRIVER_DIR}/seven_segment_driver.ko
sudo insmod ./${DRIVER_DIR}/dot_matrix_driver.ko
sudo insmod ./${DRIVER_DIR}/lcd_text_driver.ko
sudo insmod ./${DRIVER_DIR}/buzzer_driver.ko
sudo insmod ./${DRIVER_DIR}/push_switch_driver.ko

sudo lsmod