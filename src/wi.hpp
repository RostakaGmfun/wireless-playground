#pragma once

#include "wi_logger.hpp"

namespace wi
{

logger *get_logger();

#define WI_LOG_DEBUG_NOLOCK(msg, ...) get_logger()->write_nolock(logger_debug, msg, _VA_ARGS_);
#define WI_LOG_DEBUG_INFO(msg, ...) get_logger()->write_nolock(logger_info, msg, _VA_ARGS_);
#define WI_LOG_DEBUG_ERROR(msg, ...) get_logger()->write_nolock(logger_error, msg, _VA_ARGS_);

#define WI_LOG_DEBUG(msg, ...) get_logger()->write_debug(msg, _VA_ARGS_);
#define WI_LOG_INFO(msg, ...) get_logger()->write_info(msg, _VA_ARGS_);
#define WI_LOG_ERROR(msg, ...) get_logger()->write_error(msg, _VA_ARGS_);

} // namespace wi
