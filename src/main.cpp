#include "drivers/sht3x.hpp"
#include "bsp/wi_bsp.hpp"
#include "wi.hpp"

static wi::logger *global_logger;

wi::logger *get_logger()
{
    return global_logger;
}

int main()
{
    wi::bsp_init();
    global_logger = new wi::logger(1024, [] (const char *data, size_t size) {});
}
