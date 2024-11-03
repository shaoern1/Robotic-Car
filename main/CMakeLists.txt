# Add executable
add_executable(main main.c)

# Include directories for main
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/.. 
    ${CMAKE_CURRENT_LIST_DIR}/../barcode
    ${CMAKE_CURRENT_LIST_DIR}/../motor
    ${CMAKE_CURRENT_LIST_DIR}/../encoder
    ${CMAKE_CURRENT_LIST_DIR}/../config
)

# Compile definitions
target_compile_definitions(main PRIVATE
    PICO_MAX_SHARED_IRQ_HANDLERS=5
    #PICO_CYW43_SUPPORTED=1
)

# Link required libraries
target_link_libraries(main
    pico_stdlib
    hardware_adc
    motor_lib
    barcode_lib
    encoder_lib
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(main)
