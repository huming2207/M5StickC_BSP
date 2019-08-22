#pragma once

#include <driver/gpio.h>
#include <driver/i2c.h>

class axp192
{
    friend class m5stickc;
    public:
        axp192(axp192 const&) = delete;
        void operator=(axp192 const&) = delete;
        esp_err_t enable_coulomb_counter();
        esp_err_t disable_coulomb_counter();
        esp_err_t clear_coulomb_counter();
        esp_err_t get_coulomb_counter_charge(uint32_t& val_out);
        esp_err_t get_coulomb_counter_discharge(uint32_t& val_out);
        float get_remain_capacity_mah();

    private:
        axp192() = default;
        esp_err_t init();
        esp_err_t i2c_read(uint8_t reg, uint8_t *params, size_t param_len);
        esp_err_t i2c_write(uint8_t reg, uint8_t *params, size_t param_len);
        esp_err_t i2c_write_byte(uint8_t reg, uint8_t param);
        esp_err_t i2c_read_byte(uint8_t reg, uint8_t *result);
};


