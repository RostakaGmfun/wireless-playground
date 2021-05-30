#include "sensor_service.hpp"
#include "FreeRTOS.h"
#include "task.h"

#include "drivers/sht3x.hpp"
#include "drivers/apds9960.hpp"
#include "bsp/wi_bsp.hpp"
#include "port/port_gpio.hpp"
#include "wi.hpp"
#include "ble_sensor_svc.hpp"

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
    for (int addr = 0; addr < 128; addr++) {
        if (bsp_probe_i2c(addr)) {
            WI_LOG_INFO("Found I2C slave on %02x", addr);
        }
    }
    while (1) {
        int measurements_count = MEASUREMENTS_COUNT;
        driver_sht3x &sht = bsp_sht3x_get();
        bool ret = sht.trigger_measurement(sht3x_repeatability_medium, [] (void *ctx) {
                    sensor_service *p_this = static_cast<sensor_service *>(ctx);
                    xSemaphoreGive(p_this->measurement_ready_sem_);
                }, this);
        if (!ret) {
            measurements_count--;
        }

        driver_apds9960 &als = bsp_apds9960_get();
        ret = als.trigger_measurement(240, [] (void *ctx) {
                    sensor_service *p_this = static_cast<sensor_service *>(ctx);
                    xSemaphoreGive(p_this->measurement_ready_sem_);
                }, this);
        if (!ret) {
            measurements_count--;
        }

        int vbatt_mv = 0;
        int vsolar_mv = 0;
        int chip_temp = 0;
        bsp_measure_vbatt(&vbatt_mv);
        bsp_measure_vsolar(&vsolar_mv);
        bsp_measure_chip_temp(&chip_temp);

        for (int i = 0; i < measurements_count; i++) {
            if (pdTRUE != xSemaphoreTake(measurement_ready_sem_, pdMS_TO_TICKS(1000))) {
                WI_LOG_ERROR("Sensor timeout");
            }
        }

        int temp = 0;
        int rh = 0;
        if (!sht.read_data(&temp, &rh)) {
            WI_LOG_ERROR("SENSOR_DATA SHT3x read failed");
        }

        uint16_t als_r = 0;
        uint16_t als_g = 0;
        uint16_t als_b = 0;
        uint16_t als_c = 0;
        if (!als.read_data(&als_r, &als_g, &als_b, &als_c)) {
            WI_LOG_ERROR("SENSOR_DATA APDS9960 read failed");
        }

        WI_LOG_INFO("TEMP %d %d", temp, rh);
        WI_LOG_INFO("ALS %d %d %d %d", als_r, als_g, als_b, als_c);
        WI_LOG_INFO("POWER %d %d %d", vbatt_mv, vsolar_mv, chip_temp);
        ble_sensor_listener *ble_svc = ble_sensor_svc_get();
        ble_svc->update_temperature(temp);
        ble_svc->update_humidity(rh);
        ble_svc->update_als(als_r, als_g, als_b, als_c);
        vTaskDelay(pdMS_TO_TICKS(POLLING_PERIOD_MS));
    }
}

} // namespace wi
