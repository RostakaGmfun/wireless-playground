#include "sensor_service.hpp"
#include "FreeRTOS.h"
#include "task.h"

#include "drivers/sht3x.hpp"
//#include "drivers/apds9960.hpp"
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
    measurement_ready_sem_ = xSemaphoreCreateCounting(MEASUREMENTS_COUNT, 0);

    xTaskCreate(entry_fn, "sensor_service", STACK_SIZE, this, PRIORITY, &task_handle_);
}

sensor_service::~sensor_service()
{
    vTaskDelete(task_handle_);
}

void sensor_service::task_entry()
{
    while (1) {
#if 0
        driver_sht3x &sht = bsp_sht3x_get();
        sht.trigger_measurement(sht3x_repeatability_medium, [] (void *ctx) {
                    sensor_service *p_this = static_cast<sensor_service *>(ctx);
                    xSemaphoreGive(p_this->measurement_ready_sem_);
                }, this);

        driver_apds9960 &als = bsp_apds9960_get();
        als.trigger_measurement([] (void *ctx) {
                    sensor_service *p_this = static_cast<sensor_service *>(ctx);
                    xSemaphoreGive(p_this->measurement_ready_sem_);
                }, this);

        int vbatt_mv = 0;
        int vsolar_mv = 0;
        int chip_temp = 0;
        bsp_measure_vbatt(&vbatt_mv);
        bsp_measure_vsolar(&vsolar_mv);
        bsp_measure_chip_temp(&chip_temp);

        for (int i = 0; i < MEASUREMENTS_COUNT; i++) {
            xSemaphoreTake(measurement_ready_sem_, portMAX_DELAY);
        }

        int temp = 0;
        int rh = 0;
        if (!sht.read_data(&temp, &rh)) {
            WI_LOG_ERROR("SENSOR_DATA SHT3x read failed");
        }

        int als_r = 0;
        int als_g = 0;
        int als_b = 0;
        if (!als.read_data(&als_r, &als_g, &als_b)) {
            WI_LOG_ERROR("SENSOR_DATA APDS9960 read failed");
        }

        WI_LOG_INFO("SENSOR_DATA %d %d %d %d %d %d %d %d", temp, rh, als_r, als_g, als_b, vbatt_mv, vsolar_mv, chip_temp);
#endif
        vTaskDelay(pdMS_TO_TICKS(POLLING_PERIOD_MS));
    }
}

} // namespace wi
