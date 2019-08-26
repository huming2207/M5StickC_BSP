#include "bm8563.hpp"

#define BM5863_ADDR 0x51

uint8_t bm8563::get_sec()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x02) & 0x7f);
}

void bm8563::set_sec(uint8_t sec)
{
    i2c_write(BM5863_ADDR, 0x02, digit2bcd(sec) & 0x7f);
}

uint8_t bm8563::get_min()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x03) & 0x7f);
}

void bm8563::set_min(uint8_t min)
{
    i2c_write(BM5863_ADDR, 0x03, digit2bcd(min) & 0x7f);
}

uint8_t bm8563::get_hour()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x04) & 0x3f);
}

void bm8563::set_hour(uint8_t hour)
{
    i2c_write(BM5863_ADDR, 0x04, digit2bcd(hour) & 0x3f);
}

uint8_t bm8563::get_day()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x05) & 0x3f);
}

void bm8563::set_day(uint8_t day)
{
    i2c_write(BM5863_ADDR, 0x05, digit2bcd(day) & 0x3f);
}

uint8_t bm8563::get_dow()
{
    return i2c_read(BM5863_ADDR, 0x06) & 0x07;
}

void bm8563::set_dow(uint8_t dow)
{
    i2c_write(BM5863_ADDR, 0x06, dow & 0x07);
}

uint8_t bm8563::get_month()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x07) & 0x1f);
}

void bm8563::set_month(uint8_t month)
{
    i2c_write(BM5863_ADDR, 0x07, digit2bcd(month) & 0x1f);
}

uint8_t bm8563::get_year()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x08));
}

void bm8563::set_year(uint8_t year)
{
    i2c_write(BM5863_ADDR, 0x08, digit2bcd(year));
}

uint8_t bm8563::get_minute_alarm()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x09) & 0x7f);
}

void bm8563::set_minute_alarm(uint8_t minute)
{
    i2c_write(BM5863_ADDR, 0x09, digit2bcd(minute) & 0x7f);
}

void bm8563::enable_minute_alarm(bool enable)
{
    auto min = get_minute_alarm();
    i2c_write(BM5863_ADDR, 0x09, ((enable ? 0x80 : 0x00) | min));
}

uint8_t bm8563::get_hour_alarm()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x0A) & 0x3f);
}

void bm8563::set_hour_alarm(uint8_t hour)
{
    i2c_write(BM5863_ADDR, 0x0A, digit2bcd(hour) & 0x3f);
}

void bm8563::enable_hour_alarm(bool enable)
{
    auto hour = get_hour_alarm();
    i2c_write(BM5863_ADDR, 0x0A, ((enable ? 0x80 : 0x00) | hour));
}

uint8_t bm8563::get_day_alarm()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x0B) & 0x3f);
}

void bm8563::set_day_alarm(uint8_t day)
{
    i2c_write(BM5863_ADDR, 0x0B, digit2bcd(day) & 0x3f);
}

void bm8563::enable_day_alarm(bool enable)
{
    auto day = get_day_alarm();
    i2c_write(BM5863_ADDR, 0x0B, ((enable ? 0x80 : 0x00) | day));
}

uint8_t bm8563::get_dow_alarm()
{
    return bcd2digit(i2c_read(BM5863_ADDR, 0x0C) & 0x07);
}

void bm8563::set_dow_alarm(uint8_t dow)
{
    i2c_write(BM5863_ADDR, 0x0C, dow & 0x07);
}

void bm8563::enable_dow_alarm(bool enable)
{
    auto dow = get_dow_alarm();
    i2c_write(BM5863_ADDR, 0x0C, ((enable ? 0x80 : 0x00) | dow));
}

// From: https://github.com/m5stack/M5StickC/blob/master/src/RTC.cpp
uint8_t bm8563::bcd2digit(uint8_t bcd)
{
    uint8_t tmp = 0;
    tmp = ((uint8_t)(bcd & 0xF0) >> 0x4) * 10;
    return (tmp + (bcd & 0x0F));
}

uint8_t bm8563::digit2bcd(uint8_t digit)
{
    uint8_t bcd_msb = 0;

    while (digit >= 10) {
        bcd_msb++;
        digit -= 10;
    }

    return  ((uint8_t)(bcd_msb << 4) | digit);
}
