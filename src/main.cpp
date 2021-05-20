#include "drivers/sht3x.hpp"
#include "bsp/wi_bsp.hpp"
#include "wi.hpp"
#include "sensor_service.hpp"

static wi::logger *global_logger;

wi::logger *wi::get_logger()
{
    return global_logger;
}

int main()
{
    wi::bsp_init();
    global_logger = new wi::logger(1024, [] (const char *data, size_t size) {});
    wi::sensor_service *sensor_srv = new wi::sensor_service;

    vTaskStartScheduler();
}
