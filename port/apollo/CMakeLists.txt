add_library(wi_port_apollo
  port_uart.cpp
  port_i2c.cpp
  port_spi.cpp
  port_gpio.cpp
  port_flash.cpp
  port_adc.cpp
  port_itm.cpp
  port_startup.c)

target_link_libraries(wi_port_apollo PRIVATE am_sdk_hal ${WI_RTOS_LIB} wi_bsp)
target_include_directories(wi_port_apollo PUBLIC ../include)
target_link_options(wi_port_apollo
  PUBLIC
  -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
  -nostartfiles
  -static
  -eReset_Handler)
