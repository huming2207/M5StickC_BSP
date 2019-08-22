#include <cstdint>
#include <cstddef>

#include "axp192.hpp"

#define AXP192_WRITE_ADDR 0x68
#define AXP192_READ_ADDR  0x69

#define AXP192_I2C_NUM I2C_NUM_0

esp_err_t axp192::i2c_write(uint8_t reg, uint8_t *params, size_t param_len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AXP192_WRITE_ADDR, true);
    i2c_master_write(cmd, params, param_len, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(AXP192_I2C_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t axp192::i2c_write_byte(uint8_t reg, uint8_t param)
{
    return i2c_write(reg, &param, 1);
}

esp_err_t axp192::init()
{
    auto ret = i2c_write_byte(0x10, 0xFF); // OLED_VPP enable
    ret = ret ?: i2c_write_byte(0x28, 0xCC); // Enable LDO2 and LDO3, LED & TFT at 3.0v
    ret = ret ?: i2c_write_byte(0x82, 0xFF); // Enable all ADCs
    ret = ret ?: i2c_write_byte(0x33, 0xC1); // Enable charging, 200mA, 4.2v
    ret = ret ?: i2c_write_byte(0xB8, 0x80); // Enable Coulomb Counter
    ret = ret ?: i2c_write_byte(0x12, 0x4D); // Enable DC-DC1, OLED_VDD
    ret = ret ?: i2c_write_byte(0x36, 0b00001100); // Key settings, press 4s to shut down
    ret = ret ?: i2c_write_byte(0x90, 0x02); // GPIO0 setting, Low-noise LDO mode for microphone
    ret = ret ?: i2c_write_byte(0x30, 0xE0); // VBUS limit and Power path setting
    ret = ret ?: i2c_write_byte(0x39, 0xFC); // High temperature threshold
    ret = ret ?: i2c_write_byte(0x35, 0xA2); // Enable RTC battery

    return ret;
}

