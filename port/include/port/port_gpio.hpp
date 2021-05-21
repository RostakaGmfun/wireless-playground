#pragma once

#include <cstdint>
#include <cstddef>
#include "FreeRTOS.h"
#include "semphr.h"

namespace wi
{

enum port_gpio_pull_t
{
    PORT_GPIO_PULL_NONE,
    PORT_GPIO_PULL_UP,
    PORT_GPIO_PULL_DOWN,
};

enum port_gpio_dir_t
{
    PORT_GPIO_DIR_IN,
    PORT_GPIO_DIR_OUT,
};

class port_gpio
{
public:
    port_gpio(int id);
    ~port_gpio();

    port_gpio(const port_gpio &other) = delete;
    port_gpio(port_gpio &&other) = delete;
    port_gpio &operator=(const port_gpio &other) = delete;
    port_gpio &operator=(port_gpio &&other) = delete;

    void configure(int pin_id, port_gpio_dir_t dir, port_gpio_pull_t pull_type, int func);

    void set(int pin_id, bool value);
    bool get(int pin_id);

    bool lock(unsigned int timeout_ms);
    void unlock();

    int id() const
    {
        return id_;
    }

private:
    int id_;
    SemaphoreHandle_t gpio_mutex_;
};

} // namespace wi
