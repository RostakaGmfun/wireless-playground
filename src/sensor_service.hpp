#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

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
    SemaphoreHandle_t measurement_ready_sem_;

    static constexpr auto STACK_SIZE = 512;
    static constexpr auto PRIORITY = 1;
    static constexpr auto POLLING_PERIOD_MS = 1 * 1000;
    static constexpr auto MEASUREMENTS_COUNT = 2;
};

} // namespace wi
