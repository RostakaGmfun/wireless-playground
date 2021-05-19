add_library(driver_sht3x INTERFACE)
target_include_directories(driver_sht3x INTERFACE include)
target_link_libraries(driver_sht3x INTERFACE wi_port ${WI_RTOS_LIB})
