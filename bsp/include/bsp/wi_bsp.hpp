#pragma once

namespace wi
{

class driver_sht3x;

void bsp_init();

driver_sht3x *bsp_sht3x_get();

} // namespace wi
