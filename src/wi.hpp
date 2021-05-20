#pragma once

#include "wi_logger.hpp"

namespace wi
{

logger *get_logger();

#define WI_LOG_DEBUG_NOLOCK(msg, ...) get_logger()->write_nolock(logger_debug, msg, ##__VA_ARGS__)
#define WI_LOG_DEBUG_INFO(msg, ...) get_logger()->write_nolock(logger_info, msg, ##__VA_ARGS__)
#define WI_LOG_DEBUG_ERROR(msg, ...) get_logger()->write_nolock(logger_error, msg, ##__VA_ARGS__)

#define WI_LOG_DEBUG(msg, ...) get_logger()->write_debug(msg, ##__VA_ARGS__)
#define WI_LOG_INFO(msg, ...) get_logger()->write_info(msg, ##__VA_ARGS__)
#define WI_LOG_ERROR(msg, ...) get_logger()->write_error(msg, ##__VA_ARGS__)

} // namespace wi
