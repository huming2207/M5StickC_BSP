#pragma once

#include <cstdint>
#include <cstddef>
#include <driver/i2c.h>

class m5_i2c_dev
{
    protected:
        void i2c_read(uint8_t addr, uint8_t reg, uint8_t *params, size_t param_len, i2c_port_t i2c_channel = I2C_NUM_0);
        void i2c_write(uint8_t addr, uint8_t reg, uint8_t *params, size_t param_len, i2c_port_t i2c_channel = I2C_NUM_0);
        void i2c_write(uint8_t addr, uint8_t reg, uint8_t param, i2c_port_t i2c_channel = I2C_NUM_0);
        uint8_t i2c_read(uint8_t addr, uint8_t reg, i2c_port_t i2c_channel = I2C_NUM_0);
};

