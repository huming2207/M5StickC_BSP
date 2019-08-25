#include "bm8563.hpp"

#define BM5863_ADDR 0x51

uint8_t bm8563::get_sec()
{
    return i2c_read(BM5863_ADDR, 0x02) & 0x7f;
}

void bm8563::set_sec(uint8_t sec)
{
    i2c_write(BM5863_ADDR, 0x02, sec & 0x7f);
}

uint8_t bm8563::get_min()
{
    return i2c_read(BM5863_ADDR, 0x03) & 0x7f;
}

void bm8563::set_min(uint8_t min)
{
    i2c_write(BM5863_ADDR, 0x03, min & 0x7f);
}

uint8_t bm8563::get_hour()
{
    return i2c_read(BM5863_ADDR, 0x04) & 0x3f;
}

void bm8563::set_hour(uint8_t hour)
{
    i2c_write(BM5863_ADDR, 0x04, hour & 0x3f);
}

uint8_t bm8563::get_day()
{
    return i2c_read(BM5863_ADDR, 0x05) & 0x3f;
}

void bm8563::set_day(uint8_t day)
{
    i2c_write(BM5863_ADDR, 0x05, day & 0x3f);
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
    return i2c_read(BM5863_ADDR, 0x07) & 0x1f;
}

void bm8563::set_month(uint8_t month)
{
    i2c_write(BM5863_ADDR, 0x07, month & 0x1f);
}

uint8_t bm8563::get_year()
{
    return i2c_read(BM5863_ADDR, 0x08);
}

void bm8563::set_year(uint8_t year)
{
    i2c_write(BM5863_ADDR, 0x08, year);
}

uint8_t bm8563::get_minute_alarm()
{
    return 0;
}

void bm8563::set_minute_alarm(uint8_t minute)
{

}

void bm8563::enable_minute_alarm(bool enable)
{

}

uint8_t bm8563::get_hour_alarm()
{
    return 0;
}

void bm8563::set_hour_alarm(uint8_t minute)
{

}

void bm8563::enable_hour_alarm(bool enable)
{

}

uint8_t bm8563::get_day_alarm()
{
    return 0;
}

void bm8563::set_day_alarm(uint8_t minute)
{

}

void bm8563::enable_day_alarm(bool enable)
{

}

uint8_t bm8563::get_dow_alarm()
{
    return 0;
}

void bm8563::set_dow_alarm(uint8_t minute)
{

}

void bm8563::enable_dow_alarm(bool enable)
{

}
