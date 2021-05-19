#pragma once

#include "FreeRTOS.h"
#include "tasks.h"

namespace wi
{

class sensor_service
{
public:
    sensor_service();
    ~sensor_service();

private:
    void task_entry();

    TaskHandle_t task_handle_;

    static constexpr auto STACK_SIZE = 512;
    static constexpr auto PRIORITY = 1;
    static constexpr auto POLLING_PERIOD_MS = 10 * 1000;
};

} // namespace wi
