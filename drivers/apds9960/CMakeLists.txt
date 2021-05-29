add_library(driver_apds9960 INTERFACE)
target_include_directories(driver_apds9960 INTERFACE include)
target_link_libraries(driver_apds9960 INTERFACE wi_port ${WI_RTOS_LIB})
