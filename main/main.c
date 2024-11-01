#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "barcode.h"
#include "motor.h"

// Define the button pin
#define BUTTON_PIN 21

// Function to init all sensors and motors
void initAll()
{
    // Initialise standard I/O
    stdio_init_all();
    sleep_ms(1000);

    // Initialise barcode sensor pin
    init_barcode();
    printf("1/9 - Barcode sensor pin initialised\n");
    sleep_ms(500);

    // Initialize motor
    init_motor();

    // Initialize button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN); // Assuming the button is active-low

    // Initialize motor toggle buttons
    init_toggle_motor();
    init_toggle_motor_off();
}

int main()
{
    // Init all required
    initAll();
    toggle_motor();
    // Main loop
    while (1)
    {
        // Check if the button is pressed
        if (gpio_get(BUTTON_PIN) == 0) // Assuming the button is active-low
        {
            // Stop the motor
            mtr2_stop();
            printf("Button pressed, motor stopped\n");
            fflush(stdout);  // Flush the output buffer to ensure the message is sent
        }

        // Toggle motor on/off with button 22
        

        // Stop motor with button 21
        toggle_motor_off();

        // Read barcode
        printf("Reading barcode...\n");
        fflush(stdout);  // Flush the output buffer to ensure the message is sent
        track_bars();
        printf("Barcode read complete\n");
        fflush(stdout);  // Flush the output buffer to ensure the message is sent

        // Add a small delay to avoid flooding the serial monitor
        sleep_ms(1);
    }

    return 0;
}