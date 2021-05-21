#include "drivers/sht3x.hpp"
#include "bsp/wi_bsp.hpp"
#include "port/port_uart.hpp"
#include "port/port_gpio.hpp"
#include "wi.hpp"
#include "sensor_service.hpp"
#include <cstring>
#include "printf.h"

static wi::logger *global_logger;

wi::logger *wi::get_logger()
{
    return global_logger;
}

static void logger_print(const char *data, size_t size)
{
    wi::bsp_console_uart_get().write((const uint8_t *)data, size, 0xFFFFFFFF);
}

int main()
{
    wi::bsp_init();
    global_logger = new wi::logger(1024, logger_print);
    wi::sensor_service *sensor_srv = new wi::sensor_service;
    vTaskStartScheduler();
}
