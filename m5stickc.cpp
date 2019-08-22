#include <driver/i2c.h>
#include <driver/gpio.h>

#include "m5stickc.hpp"

m5stickc::m5stickc()
{
    // Initialise I2C channel 0 for PMIC and Gyro
    i2c_config_t config_0;
    config_0.mode = I2C_MODE_MASTER;
    config_0.sda_io_num = (gpio_num_t)21;
    config_0.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config_0.scl_io_num = (gpio_num_t)22;
    config_0.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config_0.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &config_0);
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, config_0.mode, 0, 0, 0));
}