#pragma once

namespace wi
{

class driver_sht3x;
class port_uart;
class port_gpio;

void bsp_init();

driver_sht3x &bsp_sht3x_get();
port_uart &bsp_console_uart_get();
port_gpio &bsp_gpio_get();

} // namespace wi
