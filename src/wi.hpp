#pragma once

#include "wi_logger.hpp"

namespace wi
{

logger *get_logger();

} // namespace wi

#define WI_LOG_DEBUG_NOLOCK(msg, ...) wi::get_logger()->write_nolock(wi::logger_debug, msg, ##__VA_ARGS__)
#define WI_LOG_INFO_NOLOCK(msg, ...) wi::get_logger()->write_nolock(wi::logger_info, msg, ##__VA_ARGS__)
#define WI_LOG_ERROR_NOLOCK(msg, ...) wi::get_logger()->write_nolock(wi::logger_error, msg, ##__VA_ARGS__)

#define WI_LOG_DEBUG(msg, ...) wi::get_logger()->write_debug(msg, ##__VA_ARGS__)
#define WI_LOG_INFO(msg, ...) wi::get_logger()->write_info(msg, ##__VA_ARGS__)
#define WI_LOG_ERROR(msg, ...) wi::get_logger()->write_error(msg, ##__VA_ARGS__)
