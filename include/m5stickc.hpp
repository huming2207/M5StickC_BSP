#pragma once

#include <axp192.hpp>

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

        m5stickc(m5stickc const&) = delete;
        void operator=(m5stickc const&) = delete;


    private:

        m5stickc();
        axp192 _axp192;
};

