#pragma once

#include <cstdint>
#include "port/port_i2c.hpp"

namespace wi
{
asd

enum sht3x_repeatability_t
{
    sht3x_repeatability_low,
    sht3x_repeatability_medium,
    sht3x_repeatability_high,
};

class driver_sht3x
{
public:
    driver_sht3x(uint8_t addr, port_i2c *i2c_port):
        addr_(addr), i2c_port_(i2c_port)
    {}

    ~driver_sht3x();

    template <typename F>
    bool trigger_measurement(sht3x_repeatability_t repeatability, F &cb)
    {

    }
};

}; // namepsace wi
