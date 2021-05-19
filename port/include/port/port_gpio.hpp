#pragma once

#include <cstdint>
#include <cstddef>

namespace wi
{

enum port_gpio_pull_t
{
    PORT_GPIO_PULL_UP,
    PORT_GPIO_PULL_DOWN,
    PORT_GPIO_PULL_NONE,
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

    void configure_direction(int pin_id, bool dir);
    void configure_pull(int pin_id, port_gpio_pull_t pull_type);
    void configure_function(int pin_id, int func);

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
};

} // namespace wi
