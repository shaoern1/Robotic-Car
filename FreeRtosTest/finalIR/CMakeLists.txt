# Add your source files here
add_executable(finalIR
    ${CMAKE_SOURCE_DIR}/finalIR/finalIR.c
)


# Don't think there is a need to change anything here
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
)

# Add required libraries
target_link_libraries(finalIR 
    pico_stdlib 
    hardware_gpio 
    hardware_timer 
    hardware_adc 
    hardware_pwm
)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(finalIR 0)
pico_enable_stdio_usb(finalIR 1)

# If using the VS code extension "Run Project" command, 
# the location of the UF2 file might match what is expected. Change the code below to match it.
set_target_properties(finalIR PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Add extra outputs if necessary
pico_add_extra_outputs(finalIR)
