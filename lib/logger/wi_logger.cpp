#include "wi_logger.hpp"

#include <ctime>
#include <sys/time.h>
#include "printf.h"

namespace wi
{

logger::logger(size_t buffer_size, logger_output_t p_output):
    buffer_(new char[buffer_size_]), buffer_size_(buffer_size),
    buffer_mutex_(xSemaphoreCreateMutex()), output_(p_output),
    level_(logger_debug)
{
}

logger::~logger()
{
    delete [] buffer_;

    if (nullptr != buffer_mutex_) {
        vSemaphoreDelete(buffer_mutex_);
    }
}

void logger::write_debug(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    write(logger_debug, format, ap);
    va_end(ap);
}

void logger::write_info(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    write(logger_info, format, ap);
    va_end(ap);
}

void logger::write_error(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    write(logger_error, format, ap);
    va_end(ap);
}

void logger::write_nolock(logger_level level, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    write_impl(level, format, ap);
    va_end(ap);
}

void logger::write(logger_level level, const char *format, va_list ap)
{
    xSemaphoreTake(buffer_mutex_, portMAX_DELAY);
    write_impl(level, format, ap);
    xSemaphoreGive(buffer_mutex_);
}

void logger::write_impl(logger_level level, const char *format, va_list ap)
{
    if (level < level_ || nullptr == output_) {
        return;
    }

    const char *level_str[] = {
        [logger_debug] = "DBG",
        [logger_info] = "INF",
        [logger_error] = "ERR",
    };

    size_t size_left = buffer_size_;

    tm tm_val;
    timeval current_time;
    gettimeofday(&current_time, nullptr);
    gmtime_r(&current_time.tv_sec, &tm_val);

    // FORMAT: LVL YYYY MM DD hh mm ss uuu MSG\n
    const int written = snprintf(buffer_, size_left, "%s %04d %02d %02d %02d %02d %02d %03d ",
            level_str[level], tm_val.tm_year, tm_val.tm_mon, tm_val.tm_mday, tm_val.tm_hour,
            tm_val.tm_min, tm_val.tm_sec, (current_time.tv_usec / 1000) % 1000);
    if (written < 0 || written == size_left) {
        return;
    }
    size_left -= written;
    const int written2 = vsnprintf(buffer_ + written, size_left, format, ap);
    if (written2 < 0 || written2 == size_left) {
        return;
    }
    size_left -= written2;
    if (size_left > 0) {
        buffer_[written + written2] = '\n';
    }

    output_(buffer_, written + written2 + 1);

    return;
}

void logger::set_level(logger_level level)
{
    level_ = level;
}

} // namespace wi
