#pragma once

#include <driver/gpio.h>
#include <driver/i2c.h>

#ifdef __cplusplus
extern "C"
{
#endif

esp_err_t axp192_m5stickc_init(i2c_port_t port);

#ifdef __cplusplus
}
#endif
