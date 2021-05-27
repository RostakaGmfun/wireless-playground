cmake_minimum_required(VERSION 3.13)
project(wi C CXX)

set(WI_TARGET_PLATFORM apollo)
set(WI_RTOS_LIB am_sdk_freertos)
set(WI_CORDIO_LIB am_sdk_cordio)

# For global configs
include_directories(include)

include(cmake/am_sdk.cmake)
include(cmake/tflite_micro.cmake)
include(cmake/ext_printf.cmake)

add_subdirectory(port)
add_subdirectory(bsp)
add_subdirectory(drivers)
add_subdirectory(lib)

add_executable(wi-fw
  src/main.cpp
  src/libc_stubs.c
  src/freertos_hooks.cpp
  src/sensor_service.cpp)

target_link_libraries(wi-fw
  wi_port
  wi_bsp
  tflite_micro
  ext_printf
  driver_sht3x
  wi_logger
  ${WI_RTOS_LIB}
  ${WI_CORDIO_LIB})

target_link_options(wi-fw PRIVATE
  "LINKER:-gc-sections"
  "LINKER:-flto"
  "LINKER:--undefined=vTaskSwitchContext")
