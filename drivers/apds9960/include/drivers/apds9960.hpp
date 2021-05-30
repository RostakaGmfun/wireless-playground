#pragma once

#include <cstdint>
#include <algorithm>
#include "port/port_i2c.hpp"

#include "FreeRTOS.h"
#include "timers.h"

namespace wi
{

class driver_apds9960
{
public:
    driver_apds9960(uint8_t addr, port_i2c &i2c_bus):
        addr_(addr), i2c_bus_(i2c_bus), cache_valid_(false)
    {
        ready_timer_ = xTimerCreate("apds9960",
                pdMS_TO_TICKS(default_timer_period_ms),
                pdFALSE,
                (void *)this, [] (TimerHandle_t timer) {
                    driver_apds9960 *p_this = (driver_apds9960 *)pvTimerGetTimerID(timer);
                    p_this->ready_callback_(p_this->ready_callback_ctx_);
                });
        vTimerSetTimerID(ready_timer_, (void *)this);
    }

    ~driver_apds9960()
    {
        xTimerDelete(ready_timer_, portMAX_DELAY);
    }

    bool trigger_measurement(uint8_t integration_time_val, void (*ready_cb)(void *ctx),
            void *ready_cb_ctx)
    {
        ready_callback_ = ready_cb;
        ready_callback_ctx_ = ready_cb_ctx;

        uint8_t value[] = { ATIME_REG, integration_time_val};
        if (!write(ATIME_REG, value, 2)) {
            return false;
        }

        uint8_t enable_value[] = { ENABLE_REG, ENABLE_PON | ENABLE_AEN};
        if (!write(ENABLE_REG, enable_value, sizeof(enable_value))) {
            return false;
        }

        adc_cycles_ = 256 - integration_time_val;
        const auto timer_period_ms = std::max(portTICK_PERIOD_MS,
                (adc_cycles_ * ADC_CYCLE_DURATION + 50) / 100 + 3);
        xTimerChangePeriod(ready_timer_, pdMS_TO_TICKS(timer_period_ms), portMAX_DELAY);

        cache_valid_ = false;

        xTimerStart(ready_timer_, portMAX_DELAY);
        return true;
    }

    bool enter_sleep()
    {
        uint8_t enable = 0;
        return write(ENABLE_REG, &enable, sizeof(enable));
    }

    bool read_data(uint16_t *p_r, uint16_t *p_g, uint16_t *p_b, uint16_t *p_c)
    {
        if (cache_valid_) {
            if (nullptr != p_r) {
                *p_r = r_cache_;
            }
            if (nullptr != p_g) {
                *p_g = g_cache_;
            }
            if (nullptr != p_b) {
                *p_b = b_cache_;
            }
            if (nullptr != p_c) {
                *p_c = c_cache_;
            }
            return true;
        }

        uint8_t status = 0;
        if (!read(STATUS_REG, &status, sizeof(status))) {
            return false;
        }

        uint8_t enable = 0;
        if (!read(ENABLE_REG, &enable, sizeof(enable))) {
            return false;
        }

        if (!(status & STATUS_AVALID)) {
            return false;
        }

        uint8_t data[4 * sizeof(uint16_t)];
        if (!read(RGB_DATA_REG, data, sizeof(data))) {
            return false;
        }

        c_cache_ = data[0] | (uint16_t)data[1] << 8;
        r_cache_ = data[2] | (uint16_t)data[3] << 8;
        g_cache_ = data[4] | (uint16_t)data[5] << 8;
        b_cache_ = data[6] | (uint16_t)data[7] << 8;
        cache_valid_ = true;

        if (nullptr != p_r) {
            *p_r = r_cache_;
        }
        if (nullptr != p_g) {
            *p_g = g_cache_;
        }
        if (nullptr != p_b) {
            *p_b = b_cache_;
        }
        if (nullptr != p_c) {
            *p_c = c_cache_;
        }

        return true;
    }

private:

    bool write(uint8_t reg, const uint8_t *value, size_t size)
    {
        if (!i2c_bus_.lock(i2c_lock_timeout_ms)) {
            return false;
        }

        i2c_bus_.configure(i2c_bus_speed_hz);

        if (!i2c_bus_.write(addr_, value, size, i2c_lock_timeout_ms)) {
            i2c_bus_.unlock();
            return false;
        }

        i2c_bus_.unlock();

        return true;
    }

    bool read(uint8_t reg, uint8_t *value, size_t size)
    {
        if (!i2c_bus_.lock(i2c_lock_timeout_ms)) {
            return false;
        }

        i2c_bus_.configure(i2c_bus_speed_hz);

        if (!i2c_bus_.write(addr_, &reg, sizeof(reg), i2c_lock_timeout_ms)) {
            i2c_bus_.unlock();
            return false;
        }

        if (!i2c_bus_.read(addr_, value, size, i2c_lock_timeout_ms)) {
            i2c_bus_.unlock();
            return false;
        }

        i2c_bus_.unlock();

        return true;
    }

    uint8_t addr_;
    port_i2c &i2c_bus_;

    TimerHandle_t ready_timer_;

    uint8_t adc_cycles_;

    bool cache_valid_;
    uint16_t r_cache_;
    uint16_t g_cache_;
    uint16_t b_cache_;
    uint16_t c_cache_;

    void (*ready_callback_)(void *);
    void *ready_callback_ctx_;

    static constexpr auto i2c_lock_timeout_ms = 100;
    static constexpr auto default_timer_period_ms = 100;
    static constexpr auto i2c_bus_speed_hz = 400000;

    static constexpr auto ADC_CYCLE_DURATION = 278LU; // 2.78ms multiplied by 100 for computation

    static constexpr uint8_t ENABLE_REG = 0x80;
    static constexpr uint8_t ENABLE_PON = 1 << 0;
    static constexpr uint8_t ENABLE_AEN = 1 << 1;

    static constexpr uint8_t ATIME_REG = 0x81;

    static constexpr uint8_t STATUS_REG = 0x93;
    static constexpr uint8_t STATUS_AVALID = 1 << 0;

    static constexpr uint8_t RGB_DATA_REG = 0x94;
};

} // namespace wi
