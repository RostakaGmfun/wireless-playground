#pragma once

#include <cstdint>
#include <cstddef>

namespace wi
{

class port_flash
{
public:
    port_flash();
    ~port_flash();

    port_flash(const port_flash &other) = delete;
    port_flash(port_flash &&other) = delete;
    port_flash &operator=(const port_flash &other) = delete;
    port_flash &operator=(port_flash &&other) = delete;

    bool write(uint32_t addr, const uint8_t *p_data, size_t size, unsigned int timeout_ms);
    bool read(uint32_t addr, uint8_t *p_data, size_t size, unsigned int timeout_ms);

    bool lock(unsigned int timeout_ms);
    void unlock();

    void enable();
    void disable();
};

} // namespace wi
