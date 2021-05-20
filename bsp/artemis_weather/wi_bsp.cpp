#include "bsp/wi_bsp.hpp"
#include "port/port_i2c.hpp"
#include "drivers/sht3x.hpp"

namespace wi
{

static driver_sht3x *sht3x;
static port_i2c *i2c_sensor_bus_;

void bsp_init()
{
    i2c_sensor_bus_ = new port_i2c(0);
    sht3x = new driver_sht3x(0x55, *i2c_sensor_bus_);
}

driver_sht3x *bsp_sht3x_get()
{
    return sht3x;
}

} // namespace wi
