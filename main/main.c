// Main program

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
//#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "barcode.h"
#include "motor.h"

// Function to init all sensors and motors
void initAll()
{
    // Initialise standard I/O
    
    sleep_ms(1000);

    // Initialise barcode sensor pin
    init_barcode();
    printf("1/9 - Barcode sensor pin initialised\n");
    sleep_ms(500);
    init_motor();
}

int main()
{
    stdio_init_all();
    // Init all required
    initAll();
    move_fwd();
    // Main loop
    while (1)
    {
        // Print message to serial monitor
        printf("Scanning...GAY\n");
        fflush(stdout);  // Flush the output buffer to ensure the message is sent
        // Debugging print to ensure loop is running
        printf("Entering track_bars function\n");
        fflush(stdout);  // Flush the output buffer to ensure the message is sent
        // Call the function to track bars
        track_bars();
        // Debugging print to ensure function call is complete
        printf("Exiting track_bars function\n");
        fflush(stdout);  // Flush the output buffer to ensure the message is sent
        // Add a delay to avoid flooding the serial monitor
        sleep_ms(1000);
    }

    return 0;
}
