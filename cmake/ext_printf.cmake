add_library(ext_printf $ENV{EXT_PRINTF_PATH}/printf.c)
target_include_directories(ext_printf PUBLIC $ENV{EXT_PRINTF_PATH})
