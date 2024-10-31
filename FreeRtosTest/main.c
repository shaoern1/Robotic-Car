#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "header.h"    // Your custom header for additional functions
#include "finalIR.h"   // Include finalIR.h to use functions from finalIR.c

// Sample main loop that calls a simple blink function from the source/blink.c file
int main()
{
    stdio_init_all();          // Initialize standard I/O (e.g., for USB output)
    cyw43_arch_init();          // Initialize CYW43 WiFi chip (if using)
    init_gpio();                // Initialize GPIO for IR sensor and reset button

    // Create FreeRTOS tasks
    xTaskCreate(led_task_cyw43, "LED_Task", 256, NULL, 1, NULL);          // Task for LED
    init_gpio();
    
    sleep_ms(1000);
    
    printf("IR Bar Pattern Analyzer (Code 39)\n");
    printf("0 = narrow bar, 1 = wide bar\n");
    printf("Using top 3 widths to determine threshold\n");
    printf("Validating for exactly %d wide bars per character\n", WIDE_BARS_PER_CHAR);
    printf("Using %d samples for denoising\n", SAMPLE_SIZE);
    printf("Reset button on GP20\n\n");
    
    track_bars();

    vTaskStartScheduler();      // Start FreeRTOS scheduler

    while(1){};                 // Infinite loop to prevent program exit (optional)
}
