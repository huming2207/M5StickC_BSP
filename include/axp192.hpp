#pragma once

#include <driver/gpio.h>
#include <driver/i2c.h>
#include "m5_i2c_dev.hpp"

namespace axp192_def
{
    typedef union __attribute__((packed)) {
        uint8_t val;
        struct __attribute__((packed)) {
            bool acin_exist:1;
            bool acin_avail:1;
            bool vbus_exist:1;
            bool vbus_avail:1;
            bool vbus_higher_vhold:1;
            bool charging:1;
            bool acin_short_to_acin:1;
            bool start_from_vbus;
        };
    } input_status;

    typedef union __attribute__((packed)) {
        uint8_t val;
        struct __attribute__((packed)) {
            bool overheated:1;
            bool charge_finished:1;
            bool battery_exist:1;
            bool _reserved_5:1;
            bool battery_activate:1;
            bool under_current_limit:1;
            bool start_mode:1;
            bool _reserved_0:1;
        };
    } mode_status;

    typedef union __attribute__((packed)) {
        uint8_t val;
        struct __attribute__((packed)) {
            uint8_t _reserved_7_3:5;
            bool vbus_en:1;
            bool vbus_session_en:1;
            bool session_end:1;
        };
    } vbus_status;
}

class axp192 : protected m5_i2c_dev
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
        uint16_t get_vin_voltage();
        uint16_t get_charge_current();
        uint16_t get_discharge_current();
        uint16_t get_ldo_2_mv();
        uint16_t get_ldo_3_mv();
        esp_err_t set_ldo_2_mv(uint16_t mv);
        esp_err_t set_ldo_3_mv(uint16_t mv);
        axp192_def::input_status get_input_status();
        axp192_def::mode_status get_mode_status();
        axp192_def::vbus_status get_vbus_status();


    private:
        axp192() = default;
        void init();
};


