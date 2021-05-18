#pragma once

#include <cstdint>
#include <cstddef>

namespace wi
{

class port_uart
{
public:
    port_uart(int id);
    ~port_uart();
    port_uart(const port_uart &other) = delete;
    port_uart(port_uart &&other) = delete;
    port_uart &operator=(const port_uart &other) = delete;
    port_uart &operator=(port_uart &&other) = delete;

    void configure(unsinged int baud);

    bool write(const uint8_t *p_data, size_t size, unsigned int timeout_ms);
    bool read(uint8_t *p_data, size_t size, unsigned int timeout_ms);

    bool lock(unsigned int timeout_ms);
    void unlock();

    void enable();
    void disable();

    int id() const
    {
        return id_;
    }

private:
    int id_;
};

} // namespace wi
