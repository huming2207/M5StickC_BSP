#include <cstdint>
#include <cstddef>
#include <esp_log.h>

#include "axp192.hpp"

#define TAG "axp192"
#define AXP192_WRITE_ADDR 0x68 // (0x34 << 1 | 0)
#define AXP192_READ_ADDR  0x69 // (0x34 << 1 | 1)

#define AXP192_I2C_NUM I2C_NUM_0

void axp192::i2c_read(uint8_t reg, uint8_t *params, size_t param_len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, AXP192_WRITE_ADDR, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg, true));

    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, AXP192_READ_ADDR, true));
    ESP_ERROR_CHECK(i2c_master_read(cmd, params, param_len, I2C_MASTER_LAST_NACK));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    ESP_ERROR_CHECK(i2c_master_cmd_begin(AXP192_I2C_NUM, cmd, pdMS_TO_TICKS(1000)));
    i2c_cmd_link_delete(cmd);
}

void axp192::i2c_write(uint8_t reg, uint8_t *params, size_t param_len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, AXP192_WRITE_ADDR, true));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg, true));
    ESP_ERROR_CHECK(i2c_master_write(cmd, params, param_len, true));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));

    ESP_ERROR_CHECK(i2c_master_cmd_begin(AXP192_I2C_NUM, cmd, pdMS_TO_TICKS(1000)));
    i2c_cmd_link_delete(cmd);
}

void axp192::i2c_write(uint8_t reg, uint8_t param)
{
    i2c_write(reg, &param, 1);
}

void axp192::i2c_read(uint8_t reg, uint8_t *result)
{
    i2c_read(reg, result, 1);
    ESP_LOGI(TAG, "Read result: 0x%x", *result);
}

void axp192::init()
{
    i2c_write(0x10, 0xFF); // OLED_VPP enable
    i2c_write(0x28, 0xCC); // Enable LDO2 and LDO3, LED & TFT at 3.0v
    i2c_write(0x82, 0xFF); // Enable all ADCs
    i2c_write(0x33, 0xC0); // Enable charging, 100mA, 4.2v
    i2c_write(0xB8, 0x80); // Enable Coulomb Counter
    i2c_write(0x12, 0x4D); // Enable DC-DC1, OLED_VDD
    i2c_write(0x36, 0b00001100); // Key settings, press 4s to shut down
    i2c_write(0x90, 0x02); // GPIO0 setting, Low-noise LDO mode for microphone
    i2c_write(0x30, 0xE0); // VBUS limit and Power path setting
    i2c_write(0x39, 0xFC); // High temperature threshold
    i2c_write(0x35, 0xA2); // Enable RTC battery
}

void axp192::enable_coulomb_counter()
{
    i2c_write(0xB8, 0x80);
}

void axp192::disable_coulomb_counter()
{
    i2c_write(0xB8, 0x00);
}

void axp192::clear_coulomb_counter()
{
    i2c_write(0xB8, 0xA0);
}

uint32_t axp192::get_coulomb_counter_charge()
{
    uint8_t buf_0 = 0, buf_1 = 0, buf_2 = 0, buf_3 = 0;
    i2c_read(0xB0, &buf_0);
    i2c_read(0xB1, &buf_1);
    i2c_read(0xB2, &buf_2);
    i2c_read(0xB3, &buf_3);

    return (uint32_t)((buf_0 << 24U) | (buf_1 << 16U) | (buf_2 << 8U) | buf_3);
}

uint32_t axp192::get_coulomb_counter_discharge()
{
    uint8_t buf_0 = 0, buf_1 = 0, buf_2 = 0, buf_3 = 0;
    i2c_read(0xB4, &buf_0);
    i2c_read(0xB5, &buf_1);
    i2c_read(0xB6, &buf_2);
    i2c_read(0xB7, &buf_3);

    return (uint32_t)((buf_0 << 24U) | (buf_1 << 16U) | (buf_2 << 8U) | buf_3);
}

float axp192::get_remain_capacity_mah()
{
    uint32_t coul_charge = get_coulomb_counter_charge();
    uint32_t coul_discharge = get_coulomb_counter_discharge();

    return (float)(65536 * 0.5 * (coul_charge - coul_discharge) / 3600.0 / 25.0);
}

uint16_t axp192::get_battery_voltage()
{
    uint8_t vbat_0 = 0, vbat_1 = 0;
    i2c_read(0x78, &vbat_0);
    i2c_read(0x79, &vbat_1);
    return (uint16_t)((vbat_0 << 4U) + vbat_1);
}

uint16_t axp192::get_usb_voltage()
{
    uint8_t vusb_0 = 0, vusb_1 = 0;
    i2c_read(0x5a, &vusb_0);
    i2c_read(0x5b, &vusb_1);
    return (uint16_t)((vusb_0 << 4U) + vusb_1);
}

uint16_t axp192::get_vin_voltage()
{
    uint8_t vin_0 = 0, vin_1 = 0;
    i2c_read(0x56, &vin_0);
    i2c_read(0x57, &vin_1);
    return (uint16_t)((vin_0 << 4U) + vin_1);
}

uint16_t axp192::get_charge_current()
{
    uint8_t byte_0 = 0, byte_1 = 0;
    i2c_read(0x7a, &byte_0);
    i2c_read(0x7b, &byte_1);
    return (uint16_t)((byte_0 << 4U) + byte_1);
}

uint16_t axp192::get_discharge_current()
{
    uint8_t byte_0 = 0, byte_1 = 0;
    i2c_read(0x7c, &byte_0);
    i2c_read(0x7d, &byte_1);
    return (uint16_t)((byte_0 << 4U) + byte_1);
}

axp192_def::input_status axp192::get_input_status()
{
    axp192_def::input_status result;
    i2c_read(0x00, &result.val);
    return result;
}

axp192_def::mode_status axp192::get_mode_status()
{
    axp192_def::mode_status result;
    i2c_read(0x01, &result.val);
    return result;
}

axp192_def::vbus_status axp192::get_vbus_status()
{
    axp192_def::vbus_status result;
    i2c_read(0x02, &result.val);
    return result;
}
