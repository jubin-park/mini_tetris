DRIVER_DIR="drivers"

echo "Update FPGA module drivers ..."

sudo rmmod dot_matrix_driver 
sudo insmod ./${DRIVER_DIR}/dot_matrix_driver.ko

sudo rmmod led_driver
sudo insmod ./${DRIVER_DIR}/led_driver.ko

sudo lsmod