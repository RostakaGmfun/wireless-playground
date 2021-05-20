add_library(wi_port INTERFACE)
target_include_directories(wi_port INTERFACE include)
add_subdirectory(${WI_TARGET_PLATFORM})
target_link_libraries(wi_port INTERFACE wi_port_${WI_TARGET_PLATFORM} ${WI_RTOS_LIB})
