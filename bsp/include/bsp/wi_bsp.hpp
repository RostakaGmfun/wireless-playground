#pragma once

namespace wi
{

class driver_sht3x;
class driver_apds9960;
class port_uart;
class port_gpio;

void bsp_init();

driver_sht3x &bsp_sht3x_get();
driver_apds9960 &bsp_apds9960_get();
port_uart &bsp_console_uart_get();
port_gpio &bsp_gpio_get();

void bsp_measure_vbatt(int *p_vbatt_mv);
void bsp_measure_vsolar(int *p_vsolar_mv);
void bsp_measure_chip_temp(int *p_chip_temp);

} // namespace wi
