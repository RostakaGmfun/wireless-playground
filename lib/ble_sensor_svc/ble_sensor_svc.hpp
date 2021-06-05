#pragma once

#include <cstdint>

namespace wi
{

/**
 * Listener interface used to update sensor readings in the service db
 */
class ble_sensor_listener
{
public:
    virtual void update_temperature(int temp_celsium) = 0;
    virtual void update_humidity(int rh) = 0;
    virtual void update_als(uint16_t r, uint16_t g, uint16_t b, uint16_t c) = 0;

    virtual void update_chip_temp(int chip_temp_celsium);
    virtual void update_vsolar(int vsolar_mv);
};

void ble_sensor_svc_init(uint32_t handler_id);

ble_sensor_listener *ble_sensor_svc_get();

} // namespace wi
