add_library(wi_bsp wi_bsp.cpp)
target_include_directories(wi_bsp PUBLIC ../include)
target_link_libraries(wi_bsp PRIVATE wi_port driver_sht3x driver_apds9960)
