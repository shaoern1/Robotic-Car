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

// External variables
extern volatile bool complete_movement;

// Function prototypes
void button_task(void *param);
void movement_task(void *param);
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

    // Initialize Ultrasonic Sensor
    ultrasonic_init();
    kalman_state *state = kalman_init(1, 100, 0, 0);

    // Initialize Button GPIO
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Use pull-up resistor

    // Create Button Handling Task
    xTaskCreate(
        button_task,            // Task function
        "ButtonTask",           // Task name
        1024,                   // Stack size (in words)
        (void *)state,          // Task parameter
        2,                      // Task priority
        NULL                    // Task handle
    );

    // Start the scheduler
    vTaskStartScheduler();

    while (1)
    {
        // Should never reach here
    }

    return 0;
}

void button_task(void *param)
{
    kalman_state *state = (kalman_state *)param;
    bool button_pressed = false;

    while (1)
    {
        // Check if button is pressed
        if (gpio_get(BUTTON_PIN) == 0)
        {
            // Debounce delay
            vTaskDelay(pdMS_TO_TICKS(50));
            if (gpio_get(BUTTON_PIN) == 0)
            {
                if (!button_pressed)
                {
                    button_pressed = true;
                    // Create Movement Task
                    xTaskCreate(
                        movement_task,       // Task function
                        "MovementTask",      // Task name
                        2048,                // Stack size (in words)
                        (void *)state,       // Task parameter
                        1,                   // Task priority
                        NULL                 // Task handle
                    );

                    // Create Distance Monitor Task
                    xTaskCreate(
                        distance_monitor_task,   // Task function
                        "DistanceMonitorTask",   // Task name
                        1024,                    // Stack size (in words)
                        NULL,                    // Task parameter
                        2,                       // Task priority
                        NULL                     // Task handle
                    );
                }
            }
        }
        else
        {
            button_pressed = false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void movement_task(void *param)
{
    kalman_state *state = (kalman_state *)param;
    double distance;

    // Move forward 90cm
    moved_distance = 0.0;
    complete_movement = false;
    start_tracking(TARGET_GRIDS); // Start tracking for 90cm
    move_grids(TARGET_GRIDS);      // Move 90cm

    // Wait until movement is complete
    while (!complete_movement) {
        vTaskDelay(pdMS_TO_TICKS(100)); // Check every 100 ms
    }

    // Stop the motor and end the task
    stop_motor();
    vTaskDelete(NULL);
}

void distance_monitor_task(void *param)
{
    (void)param;

    while (1) {
        // Get the number of grids moved
        uint32_t grids_moved = get_grids_moved(false);
        printf("Grids moved: %d\n", grids_moved);
        
        // Check if movement is complete
        if (grids_moved >= TARGET_GRIDS) {
            printf("Target distance reached. Stopping motor.\n");
            stop_motor();
            complete_movement = true;
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500 ms
    }

    // Delete the task after completion
    vTaskDelete(NULL);
}