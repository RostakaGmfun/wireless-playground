#pragma once

#include <cstdint>
#include <cstddef>

namespace wi
{

class port_uart
{
public:
    port_uart(int id, int baudrate);
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
    static constexpr auto TX_BUFFER_SIZE = 1024;
    uint8_t tx_buffer_[TX_BUFFER_SIZE];
};

} // namespace wi
