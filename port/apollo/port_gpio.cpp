#include "port/port_gpio.hpp"
#include "am_mcu_apollo.h"

namespace wi
{

port_gpio::port_gpio(int id): id_(id)
{
    gpio_mutex_ = xSemaphoreCreateMutex();
}

port_gpio::~port_gpio()
{
    vSemaphoreDelete(gpio_mutex_);
}

void port_gpio::configure(int pin_id, port_gpio_dir_t dir, port_gpio_pull_t pull_type, int func)
{
    const am_hal_gpio_pullup_e pull_mapping[] = {
        [PORT_GPIO_PULL_NONE] = AM_HAL_GPIO_PIN_PULLUP_NONE,
        [PORT_GPIO_PULL_UP] = AM_HAL_GPIO_PIN_PULLUP_WEAK,
        [PORT_GPIO_PULL_DOWN] = AM_HAL_GPIO_PIN_PULLDOWN,
    };

    const am_hal_gpio_pincfg_t config = {
        .uFuncSel = (uint32_t)func,
        .ePowerSw = AM_HAL_GPIO_PIN_POWERSW_NONE,
        .ePullup = pull_mapping[pull_type],
        .eDriveStrength = pin_id == 5 ? AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA : AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
        .eGPOutcfg = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
        .eGPInput = dir == PORT_GPIO_DIR_IN ? AM_HAL_GPIO_PIN_INPUT_ENABLE : AM_HAL_GPIO_PIN_INPUT_NONE,
        .eIntDir = AM_HAL_GPIO_PIN_INTDIR_NONE, // TODO
        .eGPRdZero = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    };

    am_hal_gpio_pinconfig(pin_id, config);
}

void port_gpio::set(int pin_id, bool value)
{
    am_hal_gpio_state_write(pin_id, value ? AM_HAL_GPIO_OUTPUT_SET : AM_HAL_GPIO_OUTPUT_CLEAR);
}

bool port_gpio::get(int pin_id)
{

}

bool port_gpio::lock(unsigned int timeout_ms)
{
    return pdTRUE == xSemaphoreTake(gpio_mutex_, pdMS_TO_TICKS(timeout_ms));
}

void port_gpio::unlock()
{
    xSemaphoreGive(gpio_mutex_);
}


} // namespace wi
