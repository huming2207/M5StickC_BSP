#pragma once

#include <driver/gpio.h>
#include <driver/i2c.h>

class axp192
{
    protected:
        axp192();

    private:
        esp_err_t i2c_write(uint8_t reg, uint8_t *params, size_t param_len);
        esp_err_t i2c_write_byte(uint8_t reg, uint8_t param);
};


