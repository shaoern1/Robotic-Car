// Main program

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "hardware/sync.h"
#include "barcode.h"
#include <FreeRTOS.h>
#include <task.h>

// Function to init all sensors and motors
void initAll()
{
    // Initialise standard I/O
    



    // Initialise barcode sensor pin
    init_barcode();
    while (1)
    {
        track_bars();
    }
    printf("1/9 - Barcode sensor pin initialised\n");
    
    

}



int main()
{
    // Init all required
    initAll();
    // create an interrupt for the barcode sensor
    

    while (1)
        tight_loop_contents();


    return 0;
}
