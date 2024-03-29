set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(TOOLCHAIN_PREFIX arm-none-eabi-)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Without that flag CMake is not able to pass test compilation check
set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nano.specs --specs=nosys.specs")

set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_DIR}${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}g++)

set(CMAKE_OBJCOPY ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")

set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -ffunction-sections -fdata-sections" CACHE STRING "GCC flags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -fno-exceptions -fno-rtti" CACHE STRING "G++ flags")
