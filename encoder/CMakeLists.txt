# Define target
set(target encoder_lib)




add_library(encoder_lib
        encoder.c
        encoder.h
        )

# pull in common dependencies
target_link_libraries(encoder_lib
    pico_stdlib 
    hardware_pwm
    motor_lib
    )

target_include_directories(encoder_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# create map/bin/hex file etc.
pico_enable_stdio_usb(encoder_lib 1)
