#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "motor.h"
#include "encoder.h"
#include "ultrasonic.h"

#define BUTTON_PIN 21      // GPIO pin connected to the button
#define TARGET_GRIDS 90    // Target distance in centimeters

// Function prototypes
void button_task(void *param);
void distance_monitor_task(void *param);
void ultrasonic_distance_task(void *param);
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

    // Initialize Ultrasonic Sensor
    ultrasonic_init();
    kalman_state *state = kalman_init(1, 100, 1, 0); // Adjusted p from 0 to 1 for better Kalman performance
    
    // Initialize Button GPIO
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Use pull-up resistor

    
    // Create Ultrasonic Sensor Task
    BaseType_t ultrasonic_result = xTaskCreate(
        ultrasonic_distance_task,  // Task function
        "UltrasonicDistance",      // Task name
        1024,                      // Stack size (in words)
        state,                     // Task parameters
        1,                         // Priority
        NULL                       // Task handle
    );


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

void ultrasonic_distance_task(void *param)
{
    kalman_state *state = (kalman_state *)param;
    double distance;

    while (1)
    {
        
        distance = ultrasonic_get_distance(state);
        printf("Current distance: %.2f cm\n", distance);
        vTaskDelay(pdMS_TO_TICKS(1)); // Print every 1 second
    }

    // Delete the task if needed
    vTaskDelete(NULL);
}