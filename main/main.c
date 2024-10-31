// Main program

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
//#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "barcode.h"
#include "motor.h"
// Define the button pin
#define BUTTON_PIN 21
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
    // use the toggle motor function to test the motor
    toggle_motor();
    
    lean_left();
    // Main loop
    while (1)
    {
       

        // Add a small delay to debounce the button
        sleep_ms(50);

        track_bars();

    }
   
    if (gpio_get(BUTTON_PIN) == 0)
    {
        stop();
    }
    return 0;
}
