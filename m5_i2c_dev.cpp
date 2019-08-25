#include "m5_i2c_dev.hpp"

#define I2C_WRITE_BIT 0
#define I2C_READ_BIT 1

void m5_i2c_dev::i2c_read(uint8_t addr, uint8_t reg, uint8_t *params, size_t param_len, i2c_port_t i2c_channel)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (addr << 1) | I2C_WRITE_BIT, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg, true));

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (addr << 1) | I2C_READ_BIT, true));
    ESP_ERROR_CHECK(i2c_master_read(cmd, params, param_len, I2C_MASTER_LAST_NACK));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_channel, cmd, pdMS_TO_TICKS(1000)));
    i2c_cmd_link_delete(cmd);
}

void m5_i2c_dev::i2c_write(uint8_t addr, uint8_t reg, uint8_t *params, size_t param_len, i2c_port_t i2c_channel)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (addr << 1) | I2C_WRITE_BIT, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg, true));
    ESP_ERROR_CHECK(i2c_master_write(cmd, params, param_len, true));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_channel, cmd, pdMS_TO_TICKS(1000)));
    i2c_cmd_link_delete(cmd);
}

void m5_i2c_dev::i2c_write(uint8_t addr, uint8_t reg, uint8_t param, i2c_port_t i2c_channel)
{
    i2c_write(addr, reg, &param, 1, i2c_channel);
}

uint8_t m5_i2c_dev::i2c_read(uint8_t addr, uint8_t reg, i2c_port_t i2c_channel)
{
    uint8_t result = 0;
    i2c_read(addr, reg, &result, 1, i2c_channel);
    return result;
}
