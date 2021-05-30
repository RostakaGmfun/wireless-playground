#pragma once

#include <cstdint>
#include <algorithm>
#include "port/port_i2c.hpp"

#include "FreeRTOS.h"
#include "timers.h"

namespace wi
{

enum sht3x_repeatability_t
{
    sht3x_repeatability_low = 0x10,
    sht3x_repeatability_medium = 0x0d,
    sht3x_repeatability_high = 0x06,
};

class driver_sht3x
{
public:
    driver_sht3x(uint8_t addr, port_i2c &i2c_bus):
        addr_(addr), i2c_bus_(i2c_bus), ready_timer_(nullptr), cache_valid_(false)
    {
        ready_timer_ = xTimerCreate("sht3x",
                pdMS_TO_TICKS(default_timer_period_ms),
                pdFALSE,
                (void *)this, [] (TimerHandle_t timer) {
                    driver_sht3x *p_this = (driver_sht3x *)pvTimerGetTimerID(timer);
                    p_this->ready_callback_(p_this->ready_callback_ctx_);
                });
        vTimerSetTimerID(ready_timer_, (void *)this);
    }

    ~driver_sht3x()
    {
        xTimerDelete(ready_timer_, portMAX_DELAY);
    }

    bool trigger_measurement(sht3x_repeatability_t repeatability, void (*ready_cb)(void *), void *ready_cb_ctx)
    {
        ready_callback_ = ready_cb;
        ready_callback_ctx_ = ready_cb_ctx;

        if (!i2c_bus_.lock(i2c_lock_timeout_ms)) {
            return false;
        }

        i2c_bus_.configure(i2c_bus_speed_hz);

        const uint8_t cmd[] = { 0x2c, repeatability };

        const bool ret = i2c_bus_.write(addr_, cmd, sizeof(cmd), i2c_lock_timeout_ms);

        i2c_bus_.unlock();

        if (!ret) {
            return ret;
        }

        switch (repeatability) {
        case sht3x_repeatability_low:
            xTimerChangePeriod(ready_timer_, pdMS_TO_TICKS(timeout_rep_low_ms), portMAX_DELAY);
            break;
        case sht3x_repeatability_medium:
            xTimerChangePeriod(ready_timer_, pdMS_TO_TICKS(timeout_rep_meidum_ms), portMAX_DELAY);
            break;
        case sht3x_repeatability_high:
            xTimerChangePeriod(ready_timer_, pdMS_TO_TICKS(timeout_rep_high_ms), portMAX_DELAY);
            break;
        }
        xTimerStart(ready_timer_, portMAX_DELAY);

        cache_valid_ = false;

        return true;
    }

    bool read_data(int *p_temp_out, int *p_rh_out)
    {
        if (cache_valid_) {
            if (nullptr != p_temp_out) {
                *p_temp_out = temp_cache_;
            }
            if (nullptr != p_rh_out) {
                *p_rh_out = rh_cache_;
            }
            return true;
        }

        if (!i2c_bus_.lock(i2c_lock_timeout_ms)) {
            return false;
        }

        i2c_bus_.configure(i2c_bus_speed_hz);

        // two 16-bit results and 1-byte checksum after each measurement
        uint8_t data[6];

        const bool ret = i2c_bus_.read(addr_, data, sizeof(data), i2c_lock_timeout_ms);

        i2c_bus_.unlock();

        if (!ret) {
            return ret;
        }

        const uint16_t temp_raw = (data[0] << 8) | data[1];
        const uint16_t rh_raw = (data[3] << 8) | data[4];

        temp_cache_ = -45 + (175LU * temp_raw) / 0xFFFF;
        rh_cache_ = (100LU * rh_raw) / 0xFFFF;
        cache_valid_ = true;

        if (nullptr != p_temp_out) {
            *p_temp_out = temp_cache_;
        }
        if (nullptr != p_rh_out) {
            *p_rh_out = rh_cache_;
        }

        return true;
    }

private:
    uint8_t addr_;
    port_i2c &i2c_bus_;

    TimerHandle_t ready_timer_;

    bool cache_valid_;
    int temp_cache_;
    int rh_cache_;

    void (*ready_callback_)(void *);
    void *ready_callback_ctx_;

    static constexpr auto i2c_lock_timeout_ms = 100;
    static constexpr auto default_timer_period_ms = 100;
    static constexpr auto i2c_bus_speed_hz = 400000;

    static constexpr auto timeout_rep_low_ms = std::max(portTICK_PERIOD_MS, 4LU);
    static constexpr auto timeout_rep_meidum_ms = std::max(portTICK_PERIOD_MS, 6LU);
    static constexpr auto timeout_rep_high_ms = std::max(portTICK_PERIOD_MS, 15LU);
};

}; // namepsace wi
