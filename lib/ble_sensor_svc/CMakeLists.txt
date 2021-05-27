add_library(ble_sensor_svc ble_sensor_svc.cpp)
target_include_directories(ble_sensor_svc PUBLIC ./)
target_link_libraries(ble_sensor_svc PRIVATE ${WI_CORDIO_LIB})
