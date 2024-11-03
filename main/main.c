#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "motor.h"
#include "encoder.h"

#define BUTTON_PIN 21      // GPIO pin connected to the button
#define TARGET_GRIDS 90    // Target distance in centimeters

// Function prototypes
void button_task(void *param);
void distance_monitor_task(void *param);

int main()
{
    // Initialize standard I/O
    stdio_init_all();
    
    cyw43_arch_init();
    // Initialize encoders
    init_encoder_setup();

    // Initialize motors
    init_motor_setup();
    init_motor_pwm();
    
    // Initialize Button GPIO
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Use pull-up resistor

    // Create Button Handling Task
    BaseType_t button_result = xTaskCreate(
        button_task,            // Task function
        "ButtonTask",           // Task name
        1024,                   // Stack size (in words)
        NULL,                   // Task parameters
        2,                      // Priority
        NULL                    // Task handle
    );

    if (button_result != pdPASS) {
        printf("Failed to create ButtonTask.\n");
    }

    // Create Distance Monitor Task
    BaseType_t distance_result = xTaskCreate(
        distance_monitor_task,  // Task function
        "DistanceMonitor",      // Task name
        1024,                   // Stack size (in words)
        NULL,                   // Task parameters
        1,                      // Priority
        NULL                    // Task handle
    );

    if (distance_result != pdPASS) {
        printf("Failed to create DistanceMonitor.\n");
    }

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Should never reach here
    while (1) {
        tight_loop_contents();
    }

    return 0;
}

/**
 * @brief Task to handle button presses on GPIO 21.
 *        Starts motor movement when button is pressed.
 */
void button_task(void *param)
{
    (void)param;
    static bool previous_state = true;
    bool current_state;

    while (1) {
        current_state = gpio_get(BUTTON_PIN);

        // Detect falling edge (button press)
        if (current_state == false && previous_state == true) {
            printf("Button pressed. Starting motor.\n");

            // Start tracking TARGET_GRIDS
            start_tracking(TARGET_GRIDS);

            // Move forward TARGET_GRIDS
            move_grids(TARGET_GRIDS);
        }

        previous_state = current_state;

        vTaskDelay(pdMS_TO_TICKS(50)); // Debounce delay
    }
}

/**
 * @brief Task to monitor the distance traveled using encoders.
 *        Stops the motor once the target distance is reached.
 */
void distance_monitor_task(void *param)
{
    (void)param;

    while (1) {
        // Get the number of grids moved
        uint32_t grids_moved = get_grids_moved(false);
        printf("Grids moved: %d\n", grids_moved);
        
        // Check if movement is complete
        if (complete_movement) {
            printf("Target distance reached. Stopping motor.\n");
            stop_motor();
            break;
            
            // Optionally, reset tracking for next operation
            start_tracking(TARGET_GRIDS);
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500 ms
    }

    // Delete the task after completion
    vTaskDelete(NULL);
}