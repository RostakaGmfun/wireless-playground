#include "drivers/sht3x.hpp"
#include "bsp/wi_bsp.hpp"
#include "port/port_uart.hpp"
#include "port/port_gpio.hpp"
#include "wi.hpp"
#include "sensor_service.hpp"
#include "ble_app.hpp"
#include <cstring>
#include <cstdint>
#include "printf.h"
#include "FreeRTOS.h"
#include "task.h"

#include "wsf_types.h"
#include "hci_core.h"
#include "hci_drv.h"
#include "hci_drv_apollo.h"
#include "hci_drv_apollo3.h"
#include "hci_apollo_config.h"

static wi::logger *global_logger;

static constexpr auto BLE_STACK_SIZE = (1024 * 4) / sizeof(uint32_t);
static constexpr auto BLE_PRIORITY = 2;

wi::logger *wi::get_logger()
{
    return global_logger;
}

static void logger_print(const char *data, size_t size)
{
    wi::bsp_console_uart_get().write((const uint8_t *)data, size, 0xFFFFFFFF);
}

static void ble_task_entry(void *pvParams)
{
    HciDrvRadioBoot(1);
    wi::ble_stack_init();
    wi::ble_stack_start();
    wi::sensor_service *sensor_srv = new wi::sensor_service;

    while (1) {
        wi::ble_stack_process_events();
    }
}

int main()
{
    wi::bsp_init();
    global_logger = new wi::logger(1024, logger_print);
    xTaskCreate(ble_task_entry, "ble_task", BLE_STACK_SIZE, nullptr, BLE_PRIORITY, nullptr);
    vTaskStartScheduler();
}
