#pragma once

#include <driver/gpio.h>
#include <driver/i2c.h>

class axp192
{
    friend class m5stickc;
    public:
        axp192(axp192 const&) = delete;
        void operator=(axp192 const&) = delete;

    private:
        axp192() = default;
        esp_err_t init();
        esp_err_t i2c_write(uint8_t reg, uint8_t *params, size_t param_len);
        esp_err_t i2c_write_byte(uint8_t reg, uint8_t param);
};


