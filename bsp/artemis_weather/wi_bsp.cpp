#include "bsp/wi_bsp.hpp"
#include "port/port_i2c.hpp"
#include "port/port_uart.hpp"
#include "port/port_gpio.hpp"
#include "drivers/sht3x.hpp"

#include "am_mcu_apollo.h"

extern "C" void am_ctimer_isr(void)
{
    uint32_t ui32Status;

    //
    // Check the timer interrupt status.
    //
    ui32Status = am_hal_ctimer_int_status_get(false);
    am_hal_ctimer_int_clear(ui32Status);

    //
    // Run handlers for the various possible timer events.
    //
    am_hal_ctimer_int_service(ui32Status);
}

namespace wi
{

static driver_sht3x *sht3x_;
static port_i2c *i2c_sensor_bus_;
static port_uart *console_uart_;
static port_gpio *gpio_;

static constexpr struct {
    int id;
    int func;
    port_gpio_dir_t dir;
    port_gpio_pull_t pull;
} pin_config[] = {
    {
        .id = 5,
        .func = AM_HAL_PIN_5_GPIO,
    },
    {
        .id = 39,
        .func = AM_HAL_PIN_39_M4SCL,
    },
    {
        .id = 40,
        .func = AM_HAL_PIN_40_M4SDAWIR3,
    },
    {
        .id = 48,
        .func = AM_HAL_PIN_48_UART0TX,
    },
    {
        .id = 49,
        .func = AM_HAL_PIN_49_UART0RX,
    },
};

void bsp_init()
{
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    am_hal_pwrctrl_low_power_init();
    am_hal_rtc_osc_disable();
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_XTAL_STOP, 0);

    gpio_ = new port_gpio(0);
    for (const auto &pin : pin_config) {
        gpio_->configure(pin.id, pin.dir, pin.pull, pin.func);
    }

    i2c_sensor_bus_ = new port_i2c(4);
    console_uart_ = new port_uart(0, 115200);
    sht3x_ = new driver_sht3x(0x44, *i2c_sensor_bus_);
}

driver_sht3x &bsp_sht3x_get()
{
    return *sht3x_;
}

port_uart &bsp_console_uart_get()
{
    return *console_uart_;
}

port_gpio &bsp_gpio_get()
{
    return *gpio_;
}

} // namespace wi
