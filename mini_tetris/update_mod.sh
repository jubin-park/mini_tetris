DRIVER_DIR="drivers"

echo "Update FPGA module drivers ..."

sudo -s

mknod /dev/csemad_led c 260 0
mknod /dev/csemad_seven_segment c 261 0
mknod /dev/csemad_dot_matrix c 262 0
mknod /dev/csemad_lcd_text c 263 0
mknod /dev/csemad_buzzer c 264 0
mknod /dev/csemad_push_switch c 265 0

rmmod led_driver
rmmod seven_segment_driver
rmmod dot_matrix_driver
rmmod lcd_text_driver
rmmod buzzer_driver
rmmod push_switch_driver

insmod ./${DRIVER_DIR}/led_driver.ko
insmod ./${DRIVER_DIR}/seven_segment_driver.ko
insmod ./${DRIVER_DIR}/dot_matrix_driver.ko
insmod ./${DRIVER_DIR}/lcd_text_driver.ko
insmod ./${DRIVER_DIR}/buzzer_driver.ko
insmod ./${DRIVER_DIR}/push_switch_driver_driver.ko

lsmod

exit