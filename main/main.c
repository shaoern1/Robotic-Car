// Main program

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "hardware/sync.h"
#include "barcode.h"
#include "motor.h" // Include motor header
#include "pico/stdlib.h"

// Function to init all sensors and motors
void initAll()
{
    // Initialise standard I/O
    stdio_init_all();

    // Initialise barcode sensor pin
    init_barcode();
    printf("1/9 - Barcode sensor pin initialised\n");

    // Initialise motor
    init_motor();
    printf("2/9 - Motor initialised\n");
}

int main()
{
    // Init all required
    initAll();

    // Variables to keep track of time
    absolute_time_t last_barcode_check = get_absolute_time();
    absolute_time_t last_motor_move = get_absolute_time();

    while (1)
    {
        // Check barcode sensor every 100 ms
        if (absolute_time_diff_us(last_barcode_check, get_absolute_time()) >= 100000)
        {
            track_bars();
            last_barcode_check = get_absolute_time();
        }

        // Move motor forward every 100 ms
        if (absolute_time_diff_us(last_motor_move, get_absolute_time()) >= 100000)
        {
            move_fwd();
            last_motor_move = get_absolute_time();
        }

        // Allow other processes to run
        tight_loop_contents();
    }

    return 0;
}