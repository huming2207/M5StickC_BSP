#include <cstdint>
#include <cstddef>
#include <esp_log.h>

#include "axp192.hpp"

#define TAG "axp192"
#define AXP192_ADDR 0x34

#define AXP192_I2C_NUM I2C_NUM_0

void axp192::init()
{
    i2c_write(AXP192_ADDR, 0x10, 0xFF); // OLED_VPP enable
    i2c_write(AXP192_ADDR, 0x28, 0xCC); // Enable LDO2 and LDO3, LED & TFT at 3.0v
    i2c_write(AXP192_ADDR, 0x82, 0xFF); // Enable all ADCs
    i2c_write(AXP192_ADDR, 0x33, 0xC0); // Enable charging, 100mA, 4.2v
    i2c_write(AXP192_ADDR, 0xB8, 0x80); // Enable Coulomb Counter
    i2c_write(AXP192_ADDR, 0x12, 0x4D); // Enable DC-DC1, OLED_VDD
    i2c_write(AXP192_ADDR, 0x36, 0b00001100); // Key settings, press 4s to shut down
    i2c_write(AXP192_ADDR, 0x90, 0x02); // GPIO0 setting, Low-noise LDO mode for microphone
    i2c_write(AXP192_ADDR, 0x30, 0xE0); // VBUS limit and Power path setting
    i2c_write(AXP192_ADDR, 0x39, 0xFC); // High temperature threshold
    i2c_write(AXP192_ADDR, 0x35, 0xA2); // Enable RTC battery
}

void axp192::enable_coulomb_counter()
{
    i2c_write(AXP192_ADDR, 0xB8, 0x80);
}

void axp192::disable_coulomb_counter()
{
    i2c_write(AXP192_ADDR, 0xB8, 0x00);
}

void axp192::clear_coulomb_counter()
{
    i2c_write(AXP192_ADDR, 0xB8, 0xA0);
}

uint32_t axp192::get_coulomb_counter_charge()
{
    return (uint32_t)((i2c_read(AXP192_ADDR, 0xB0) << 24U) | (i2c_read(AXP192_ADDR, 0xB1) << 16U) |
                        (i2c_read(AXP192_ADDR, 0xB2) << 8U) | i2c_read(AXP192_ADDR, 0xB3));
}

uint32_t axp192::get_coulomb_counter_discharge()
{
    return (uint32_t)((i2c_read(AXP192_ADDR, 0xB4) << 24U) | (i2c_read(AXP192_ADDR, 0xB5) << 16U) |
                      (i2c_read(AXP192_ADDR, 0xB6) << 8U) | i2c_read(AXP192_ADDR, 0xB7));
}

float axp192::get_remain_capacity_mah()
{
    uint32_t coul_charge = get_coulomb_counter_charge();
    uint32_t coul_discharge = get_coulomb_counter_discharge();

    return (float)(65536 * 0.5 * (coul_charge - coul_discharge) / 3600.0 / 25.0);
}

uint16_t axp192::get_battery_voltage()
{
    return (uint16_t)(((i2c_read(AXP192_ADDR, 0x78) << 4U) + i2c_read(AXP192_ADDR, 0x79)) * 1.1);
}

uint16_t axp192::get_usb_voltage()
{
    return (uint16_t)(((i2c_read(AXP192_ADDR, 0x5a) << 4U) + i2c_read(AXP192_ADDR, 0x5b)) * 1.7);
}

uint16_t axp192::get_vin_voltage()
{
    return (uint16_t)(((i2c_read(AXP192_ADDR, 0x56) << 4U) + i2c_read(AXP192_ADDR, 0x57)) * 1.7);
}

uint16_t axp192::get_charge_current()
{
    return (uint16_t)(((i2c_read(AXP192_ADDR, 0x7a) << 4U) + i2c_read(AXP192_ADDR, 0x7b)) / 2);
}

uint16_t axp192::get_discharge_current()
{
    return (uint16_t)(((i2c_read(AXP192_ADDR, 0x7c) << 4U) + i2c_read(AXP192_ADDR, 0x7d)) / 2);
}

axp192_def::input_status axp192::get_input_status()
{
    axp192_def::input_status result;
    result.val = i2c_read(AXP192_ADDR, 0x00);
    return result;
}

axp192_def::mode_status axp192::get_mode_status()
{
    axp192_def::mode_status result;
    result.val = i2c_read(AXP192_ADDR, 0x01);
    return result;
}

axp192_def::vbus_status axp192::get_vbus_status()
{
    axp192_def::vbus_status result;
    result.val = i2c_read(AXP192_ADDR, 0x02);
    return result;
}
