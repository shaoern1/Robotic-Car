add_executable(project
        project.c
        )

# pull in common dependencies and additional spi hardware support
target_link_libraries(project pico_stdlib hardware_spi hardware_i2c)

pico_enable_stdio_usb(project 1)
pico_enable_stdio_uart(project 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(project)

# add url via pico_set_program_url
example_auto_set_url(project)
