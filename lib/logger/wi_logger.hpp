#pragma once

#include <cstdarg>
#include <cstddef>
#include "FreeRTOS.h"
#include "semphr.h"

namespace wi
{

enum logger_level
{
    logger_debug,
    logger_info,
    logger_error,
};

typedef void (*logger_output_t)(const char *data, size_t size);

class logger
{
public:
    logger(size_t buffer_size, logger_output_t p_output);
    ~logger();

    logger(const logger &other) = delete;
    logger &operator=(const logger &other) = delete;

    void write_nolock(logger_level level, const char *format, ...);

    void write_debug(const char *format, ...);
    void write_info(const char *format, ...);
    void write_error(const char *format, ...);

    void write(logger_level level, const char *format, va_list ap);

    void set_level(logger_level level);

private:

    void write_impl(logger_level level, const char *format, va_list ap);

    char *buffer_;
    size_t buffer_size_;
    SemaphoreHandle_t buffer_mutex_;
    logger_output_t output_;
    logger_level level_;
};

} // namespace wi
