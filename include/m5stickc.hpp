#pragma once

#include <string>

#include <axp192.hpp>
#include <functional>
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
        void set_led(bool en);
        void toggle_led();

        void on_btn_a_press(const std::function<void()>& func);
        void on_btn_b_press(const std::function<void()>& func);
        void on_btn_a_release(const std::function<void()>& func);
        void on_btn_b_release(const std::function<void()>& func);

    private:
        m5stickc();
        static void on_btn_intr(void *ctx);

        static xQueueHandle button_evt_queue;
        static void btn_a_isr_handler(void* arg);
        static void btn_b_isr_handler(void* arg);

        axp192 _axp192;
        bm8563 _bm8563;

        std::function<void()> btn_a_press_cb{};
        std::function<void()> btn_b_press_cb{};
        std::function<void()> btn_a_release_cb{};
        std::function<void()> btn_b_release_cb{};

        static const gpio_num_t BUTTON_A_GPIO = GPIO_NUM_35;
        static const gpio_num_t BUTTON_B_GPIO = GPIO_NUM_37;
};

