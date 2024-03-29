#include <ctime>
#include <functional>

#include <sys/time.h>
#include <esp_sntp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <driver/i2c.h>
#include <driver/gpio.h>

#include "m5stickc.hpp"

#define TAG "m5stickc"
#define BUTTON_A_IO GPIO_NUM_37
#define BUTTON_B_IO GPIO_NUM_39

xQueueHandle m5stickc::button_evt_queue = nullptr;

void IRAM_ATTR m5stickc::btn_a_isr_handler(void* arg)
{
    uint32_t gpio_num = (long)arg;
    xQueueSendFromISR(button_evt_queue, &gpio_num, nullptr);
}

void IRAM_ATTR m5stickc::btn_b_isr_handler(void* arg)
{
    uint32_t gpio_num = (long)arg;
    xQueueSendFromISR(button_evt_queue, &gpio_num, nullptr);
}

m5stickc::m5stickc() : _axp192(), _bm8563()
{
    // Initialise I2C channel 0 for PMIC and Gyro
    i2c_config_t config_0;
    config_0.mode = I2C_MODE_MASTER;
    config_0.sda_io_num = (gpio_num_t)21;
    config_0.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config_0.scl_io_num = (gpio_num_t)22;
    config_0.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config_0.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &config_0);
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, config_0.mode, 0, 0, 0));

    ESP_LOGI(TAG, "I2C_0 initialised");

    // Initialise PMIC
    _axp192.init();
    ESP_LOGI(TAG, "PMIC initialised");

    // Initialise on-board red LED
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_10);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(GPIO_NUM_10, 1);

    // Initialise button interrupt
    gpio_config_t btn_conf;
    btn_conf.intr_type = GPIO_INTR_ANYEDGE;
    btn_conf.mode = GPIO_MODE_INPUT;
    btn_conf.pin_bit_mask = (1ULL << GPIO_NUM_37) | (1ULL << GPIO_NUM_39);
    btn_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    btn_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&btn_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_A_IO, btn_a_isr_handler, (void*)BUTTON_A_IO);
    gpio_isr_handler_add(BUTTON_B_IO, btn_b_isr_handler, (void*)BUTTON_B_IO);

    // Initialise queue for button interrupt
    button_evt_queue = xQueueCreate(10, sizeof(gpio_num_t));
    xTaskCreate(m5stickc::on_btn_intr, "btn_intr", 2048, this, 10, nullptr);
}

void m5stickc::load_time(const std::string& tz)
{
    struct tm time_info{};
    time_info.tm_hour = _bm8563.get_hour();
    time_info.tm_mday = _bm8563.get_day();
    time_info.tm_mon = _bm8563.get_month() - 1;
    time_info.tm_min = _bm8563.get_min();
    time_info.tm_sec = _bm8563.get_sec();
    time_info.tm_year = _bm8563.get_year() + 100;

    setenv("TZ", tz.c_str(), 1);
    tzset();

    auto rtc_timestamp = mktime(&time_info);
    ESP_LOGD(TAG, "load_time Timestamp: %lu", rtc_timestamp);

    struct timeval time_now{};
    time_now.tv_sec = rtc_timestamp;

    settimeofday(&time_now, nullptr);
}

void m5stickc::save_time(const std::string& tz)
{
    // Get current time
    setenv("TZ", tz.c_str(), 1);
    tzset();

    time_t curr_time = time(nullptr);
    struct tm result_tm{};
    struct tm *curr_tm = localtime_r(&curr_time, &result_tm);

    ESP_LOGD(TAG, "save_time Timestamp: %lu", curr_time);

    // Write to RTC
    _bm8563.set_day(curr_tm->tm_mday);
    _bm8563.set_dow(curr_tm->tm_wday);
    _bm8563.set_year(curr_tm->tm_year - 100); // Yea we are in 21st century...
    _bm8563.set_month(curr_tm->tm_mon + 1); // POSIX tm month is starting from 0...
    _bm8563.set_min(curr_tm->tm_min);
    _bm8563.set_hour(curr_tm->tm_hour);
    _bm8563.set_sec(curr_tm->tm_sec);
}

void m5stickc::set_led(bool en)
{
    gpio_set_level(GPIO_NUM_10, en ? 0 : 1);
}

void m5stickc::toggle_led()
{
    gpio_set_level(GPIO_NUM_10, gpio_get_level(GPIO_NUM_10) == 1 ? 0 : 1);
}

void m5stickc::on_btn_a_press(const std::function<void()>& func)
{
    btn_a_press_cb = func;
}

void m5stickc::on_btn_b_press(const std::function<void()>& func)
{
    btn_b_press_cb = func;
}

void m5stickc::on_btn_a_release(const std::function<void()>& func)
{
    btn_a_release_cb = func;
}

void m5stickc::on_btn_b_release(const std::function<void()>& func)
{
    btn_b_release_cb = func;
}

void m5stickc::on_btn_intr(void *ctx)
{
    gpio_num_t gpio_num = GPIO_NUM_0;
    auto *instance = static_cast<m5stickc*>(ctx);
    while(true) {
        if(xQueueReceive(button_evt_queue, &gpio_num, portMAX_DELAY)) {
            if(instance == nullptr) return;
            if(gpio_num == BUTTON_A_IO) {
                if(gpio_get_level(BUTTON_A_IO) == 0) instance->btn_a_press_cb();
                else instance->btn_a_release_cb();
            } else if(gpio_num == BUTTON_B_IO) {
                if(gpio_get_level(BUTTON_B_IO) == 0) instance->btn_b_press_cb();
                else instance->btn_b_release_cb();
            }
        }
    }
}

void m5stickc::set_brightness(uint8_t val)
{
    if(val > 12) val = 12; // The LDO adjustable range is 1.8 to 3.3v, but we need to Limit to 3.0v
    uint16_t mv = val * 100 + 1800;
    _axp192.set_ldo_2_mv(mv);
}


