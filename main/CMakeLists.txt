add_executable(main
        main.c
        #${CMAKE_CURRENT_LIST_DIR}/../wifi/cgi.c 
        #${CMAKE_CURRENT_LIST_DIR}/../wifi/ssi.c
        )

target_include_directories(main PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${CMAKE_CURRENT_LIST_DIR}/../barcode
        ${CMAKE_CURRENT_LIST_DIR}/../motor
        ${CMAKE_CURRENT_LIST_DIR}/../encoder
        ${CMAKE_CURRENT_LIST_DIR}/../config
        # use C:\Users\azree\OneDrive\Desktop\FreeRTOS-Kernel-main\include for FREERTOS_KERNEL_PATH
        ${FREERTOS_KERNEL_PATH}/include
        )
target_compile_definitions(main PRIVATE
        PICO_MAX_SHARED_IRQ_HANDLERS=5
        )

# pull in common dependencies
target_link_libraries(main 
        pico_cyw43_arch_lwip_threadsafe_background
        #pico_lwip_http
        pico_stdlib
        hardware_adc
        motor_lib
        barcode_lib
        encoder_lib
        
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(main)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)