#include <cstdint>
#include <cstddef>

#include "axp192.hpp"

#define AXP192_WRITE_ADDR 0x68 // (0x34 << 1 | 0)
#define AXP192_READ_ADDR  0x69 // (0x34 << 1 | 1)

#define AXP192_I2C_NUM I2C_NUM_0

esp_err_t axp192::i2c_read(uint8_t reg, uint8_t *params, size_t param_len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, AXP192_WRITE_ADDR, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, AXP192_READ_ADDR, true));
    ESP_ERROR_CHECK(i2c_master_read(cmd, params, param_len, I2C_MASTER_ACK));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    ESP_ERROR_CHECK(i2c_master_cmd_begin(AXP192_I2C_NUM, cmd, pdMS_TO_TICKS(1000)));
    i2c_cmd_link_delete(cmd);
    return ESP_OK;
}

esp_err_t axp192::i2c_write(uint8_t reg, uint8_t *params, size_t param_len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, AXP192_WRITE_ADDR, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg, true));
    ESP_ERROR_CHECK(i2c_master_write(cmd, params, param_len, true));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    ESP_ERROR_CHECK(i2c_master_cmd_begin(AXP192_I2C_NUM, cmd, pdMS_TO_TICKS(1000)));
    i2c_cmd_link_delete(cmd);
    return ESP_OK;
}

esp_err_t axp192::i2c_write_byte(uint8_t reg, uint8_t param)
{
    return i2c_write(reg, &param, 1);
}

esp_err_t axp192::i2c_read_byte(uint8_t reg, uint8_t *result)
{
    return i2c_read(reg, result, 1);
}

esp_err_t axp192::init()
{
    auto ret = i2c_write_byte(0x10, 0xFF); // OLED_VPP enable
    ret = ret ?: i2c_write_byte(0x28, 0xCC); // Enable LDO2 and LDO3, LED & TFT at 3.0v
    ret = ret ?: i2c_write_byte(0x82, 0xFF); // Enable all ADCs
    ret = ret ?: i2c_write_byte(0x33, 0xC0); // Enable charging, 100mA, 4.2v
    ret = ret ?: i2c_write_byte(0xB8, 0x80); // Enable Coulomb Counter
    ret = ret ?: i2c_write_byte(0x12, 0x4D); // Enable DC-DC1, OLED_VDD
    ret = ret ?: i2c_write_byte(0x36, 0b00001100); // Key settings, press 4s to shut down
    ret = ret ?: i2c_write_byte(0x90, 0x02); // GPIO0 setting, Low-noise LDO mode for microphone
    ret = ret ?: i2c_write_byte(0x30, 0xE0); // VBUS limit and Power path setting
    ret = ret ?: i2c_write_byte(0x39, 0xFC); // High temperature threshold
    ret = ret ?: i2c_write_byte(0x35, 0xA2); // Enable RTC battery

    return ret;
}

esp_err_t axp192::enable_coulomb_counter()
{
    return i2c_write_byte(0xB8, 0x80);
}

esp_err_t axp192::disable_coulomb_counter()
{
    return i2c_write_byte(0xB8, 0x00);
}

esp_err_t axp192::clear_coulomb_counter()
{
    return i2c_write_byte(0xB8, 0xA0);
}

esp_err_t axp192::get_coulomb_counter_charge(uint32_t& val_out)
{
    uint8_t buf_0 = 0, buf_1 = 0, buf_2 = 0, buf_3 = 0;
    auto ret = i2c_read_byte(0xB0, &buf_0);
    ret = ret ?: i2c_read_byte(0xB1, &buf_1);
    ret = ret ?: i2c_read_byte(0xB2, &buf_2);
    ret = ret ?: i2c_read_byte(0xB3, &buf_3);

    val_out = ((buf_0 << 24U) | (buf_1 << 16U) | (buf_2 << 8U) | buf_3);
    return ret;
}

esp_err_t axp192::get_coulomb_counter_discharge(uint32_t& val_out)
{
    uint8_t buf_0 = 0, buf_1 = 0, buf_2 = 0, buf_3 = 0;
    auto ret = i2c_read_byte(0xB4, &buf_0);
    ret = ret ?: i2c_read_byte(0xB5, &buf_1);
    ret = ret ?: i2c_read_byte(0xB6, &buf_2);
    ret = ret ?: i2c_read_byte(0xB7, &buf_3);

    val_out = ((buf_0 << 24U) | (buf_1 << 16U) | (buf_2 << 8U) | buf_3);
    return ret;
}

float axp192::get_remain_capacity_mah()
{
    uint32_t coul_charge = 0, coul_discharge = 0;
    if(get_coulomb_counter_charge(coul_charge) != ESP_OK) return -1;
    if(get_coulomb_counter_discharge(coul_discharge) != ESP_OK) return -1;

    return (float)(65536 * 0.5 * (coul_charge - coul_discharge) / 3600.0 / 25.0);
}

esp_err_t axp192::get_battery_voltage(uint16_t& vbat_mv)
{
    uint8_t vbat_0 = 0, vbat_1 = 0;
    auto ret = i2c_read(0x78, &vbat_0, 1);
    ret = ret ?: i2c_read(0x79, &vbat_1, 1);
    vbat_mv = ((vbat_0 << 4U) | vbat_1);
    return ret;
}

esp_err_t axp192::get_usb_voltage(uint16_t& input_mv)
{
    uint8_t vbat_0 = 0, vbat_1 = 0;
    auto ret = i2c_read(0x5a, &vbat_0, 1);
    ret = ret ?: i2c_read(0x5b, &vbat_1, 1);
    input_mv = ((vbat_0 << 4U) | vbat_1);
    return ret;
}