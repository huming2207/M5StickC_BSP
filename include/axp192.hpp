#pragma once

#include <driver/gpio.h>
#include <driver/i2c.h>

class axp192
{
    friend class m5stickc;
    public:
        axp192(axp192 const&) = delete;
        void operator=(axp192 const&) = delete;
        void enable_coulomb_counter();
        void disable_coulomb_counter();
        void clear_coulomb_counter();
        uint32_t get_coulomb_counter_charge();
        uint32_t get_coulomb_counter_discharge();
        float get_remain_capacity_mah();
        uint16_t get_battery_voltage();
        uint16_t get_usb_voltage();

    private:
        axp192() = default;
        void init();
        void i2c_read(uint8_t reg, uint8_t *params, size_t param_len);
        void i2c_write(uint8_t reg, uint8_t *params, size_t param_len);
        void i2c_write_byte(uint8_t reg, uint8_t param);
        void i2c_read_byte(uint8_t reg, uint8_t *result);
};


