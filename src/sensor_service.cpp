#include "sensor_service.hpp"
#include "FreeRTOS.h"
#include "task.h"

#include "drivers/sht3x.hpp"
#include "bsp/wi_bsp.hpp"
#include "port/port_gpio.hpp"
#include "wi.hpp"

namespace wi
{

sensor_service::sensor_service()
{
    auto entry_fn = [] (void *ctx) {
        sensor_service *p_this = static_cast<sensor_service *>(ctx);
        p_this->task_entry();
    };
    measurement_ready_sem_ = xSemaphoreCreateCounting(1, 0);

    xTaskCreate(entry_fn, "sensor_service", STACK_SIZE, this, PRIORITY, &task_handle_);
    wi::bsp_gpio_get().set(5, true);
}

sensor_service::~sensor_service()
{
    vTaskDelete(task_handle_);
}

void sensor_service::task_entry()
{
    bool flag = false;
    while (1) {
        WI_LOG_INFO("SENSOR_DATA");
        bsp_gpio_get().set(5, flag);
        flag = !flag;
        /*
        driver_sht3x &p_sht = bsp_sht3x_get();
        p_sht.trigger_measurement(sht3x_repeatability_medium, [] (void *ctx) {
                    sensor_service *p_this = static_cast<sensor_service *>(ctx);
                    xSemaphoreGive(p_this->measurement_ready_sem_);
                }, this);
        xSemaphoreTake(measurement_ready_sem_, portMAX_DELAY);
        int temp = 0;
        int rh = 0;
        if (p_sht.read_data(&temp, &rh)) {
            WI_LOG_INFO("SENSOR_DATA %d %d", temp, rh);
        } else {
            WI_LOG_ERROR("SENSOR_DATA read failed");
        }
        */
        vTaskDelay(pdMS_TO_TICKS(POLLING_PERIOD_MS));
    }
}

} // namespace wi
