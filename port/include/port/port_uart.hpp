#pragma once

#include <cstdint>
#include <cstddef>

namespace wi
{

class port_uart
{
public:
    port_uart(int id, uint32_t baudrate);
    ~port_uart();
    port_uart(const port_uart &other) = delete;
    port_uart(port_uart &&other) = delete;
    port_uart &operator=(const port_uart &other) = delete;
    port_uart &operator=(port_uart &&other) = delete;

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
    void *uart_handle_;
};

} // namespace wi
