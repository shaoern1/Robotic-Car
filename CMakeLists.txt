cmake_minimum_required(VERSION 3.25)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)


project(RobotCar C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

set(PICO_BOARD pico_w CACHE STRING "Board type")


# Include the FreeRTOS kernel
include(FreeRTOS_Kernel_import.cmake)

# Add the config directory to include FreeRTOSConfig.h and lwipopts.h
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/config)

# Add the FreeRTOS source directory to the include directories
include_directories(${FREERTOS_KERNEL_PATH}/include)
include_directories(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040)
include_directories(${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0)  # Add this line

if (NOT PICO_NO_HARDWARE)
    add_subdirectory(main)
    add_subdirectory(motor)
    add_subdirectory(barcode)
    add_subdirectory(encoder)
    
endif ()
