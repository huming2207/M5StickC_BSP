#pragma once

#include <cstdint>
#include "m5_i2c_dev.hpp"

class bm8563 : protected m5_i2c_dev
{
    friend class m5stickc;
    public:
        uint8_t get_sec();
        void set_sec(uint8_t sec);
        uint8_t get_min();
        void set_min(uint8_t min);
        uint8_t get_hour();
        void set_hour(uint8_t hour);
        uint8_t get_day();
        void set_day(uint8_t day);
        uint8_t get_dow();
        void set_dow(uint8_t dow);
        uint8_t get_month();
        void set_month(uint8_t month);
        uint8_t get_year();
        void set_year(uint8_t year);

        uint8_t get_minute_alarm();
        void set_minute_alarm(uint8_t minute);
        void enable_minute_alarm(bool enable);
        uint8_t get_hour_alarm();
        void set_hour_alarm(uint8_t hour);
        void enable_hour_alarm(bool enable);
        uint8_t get_day_alarm();
        void set_day_alarm(uint8_t day);
        void enable_day_alarm(bool enable);
        uint8_t get_dow_alarm();
        void set_dow_alarm(uint8_t dow);
        void enable_dow_alarm(bool enable);

    private:
        bm8563() = default;
        static uint8_t bcd2digit(uint8_t bcd);
        static uint8_t digit2bcd(uint8_t digit);
};
