# Define target
set(target ultrasonic_lib)

# Add library target for source file
add_library(${target}
ultrasonic.c
ultrasonic.h
)



# Include directories for the library
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR})


# pull in common dependencies
target_link_libraries(${target}
    pico_stdlib 
    hardware_gpio 
    hardware_timer)

pico_enable_stdio_usb(ultrasonic_lib 1)
pico_enable_stdio_uart(ultrasonic_lib 0)
