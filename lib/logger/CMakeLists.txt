add_library(wi_logger wi_logger.cpp)
target_include_directories(wi_logger PUBLIC ./)
target_link_libraries(wi_logger PRIVATE ext_printf ${WI_RTOS_LIB})
