# Add your source files here
add_executable(${PROJECT_NAME}
    main.c
   
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
   ${CMAKE_CURRENT_LIST_DIR}
   # add common folder directory
    ${CMAKE_CURRENT_LIST_DIR}/../common
)

# pull in common dependencies
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_pwm)

# Enable output over UART/USB
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})