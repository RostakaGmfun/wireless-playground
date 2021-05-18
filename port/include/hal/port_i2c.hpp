#pragma once

#include <cstdint>
#include <cstddef>

namespace wi
{

class port_i2c
{
public:
    port_i2c(int id);
    ~port_i2c();

    port_i2c(const port_i2c &other) = delete;
    port_i2c(port_i2c &&other) = delete;
    port_i2c &operator=(const port_i2c &other) = delete;
    port_i2c &operator=(port_i2c &&other) = delete;

    void configure(unsigned int clock_speed_hz);

    bool write(uint8_t addr, const uint8_t *p_data, size_t size, unsigned int timeout_ms);
    bool read(uint8_t addr, uint8_t *p_data, size_t size, unsigned int timeout_ms);

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
