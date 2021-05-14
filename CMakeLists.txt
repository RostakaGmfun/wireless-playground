cmake_minimum_required(VERSION 3.13)
project(lp-ble-platform C CXX)

include_directories(include)

include(cmake/am_sdk.cmake)
include(cmake/tflite_micro.cmake)
include(cmake/ext_printf.cmake)

set(WI_TARGET_PLATFORM apollo)

add_subdirectory(port)

add_executable(lp-ble-fw
  src/main.cpp
  src/libc_stubs.c)

target_link_libraries(lp-ble-fw
  wi_port
  tflite_micro
  ext_printf)
