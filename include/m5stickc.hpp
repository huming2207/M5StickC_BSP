#pragma once

#include <string>

#include <axp192.hpp>
#include "bm8563.hpp"

class m5stickc
{
    public:
        static m5stickc& get_bsp()
        {
            static m5stickc instance;
            return instance;
        }

        axp192& pmic()
        {
            return _axp192;
        }

        bm8563& rtc()
        {
            return _bm8563;
        }

        m5stickc(m5stickc const&) = delete;
        void operator=(m5stickc const&) = delete;

    public:
        void load_time(const std::string& tz = "AEST-10AEDT,M10.1.0,M4.1.0/3"); // Use Australia/Melbourne timezone for testing
        void save_time(const std::string& tz = "AEST-10AEDT,M10.1.0,M4.1.0/3");

    private:
        m5stickc();
        axp192 _axp192;
        bm8563 _bm8563;
};

