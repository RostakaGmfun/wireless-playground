cmake_minimum_required(VERSION 3.0.2)
project(lp-ble-platform C CXX)

include_directories(include)

include(cmake/am_sdk.cmake)

add_executable(lp-ble-fw src/main.cpp)
target_link_libraries(lp-ble-fw am_sdk_hal am_sdk_freertos am_sdk_cordio)
