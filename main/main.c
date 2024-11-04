#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "motor.h"
#include "encoder.h"
#include "ultrasonic.h"

#define BUTTON_PIN 21      // GPIO pin connected to the button
// #define TARGET_GRIDS 90    // Target distance in centimeters

// Function prototypes
void button_task(void *param);
void movement_task(void *param);
<<<<<<< HEAD
void distance_monitor_task(void *param);
void print_distance_task(void *param);

=======
>>>>>>> parent of ad286d0 (Revert back to debug ultrasonic)

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

    // Create Button Handling Task
    xTaskCreate(
        button_task,            // Task function
        "ButtonTask",           // Task name
        1024,                   // Stack size (in words)
        (void *)state,          // Task parameter
        2,                      // Task priority
        NULL                    // Task handle
    );

    // Create Print Distance Task
    xTaskCreate(
        print_distance_task,    // Task function
        "PrintDistanceTask",    // Task name
        1024,                   // Stack size (in words)
        (void *)state,          // Task parameter
        1,                      // Task priority
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
    uint64_t pulse_width;

    // Move forward until an object is within 10cm
    while (1)
    {
        distance = ultrasonic_get_distance(state);
        printf("Distance: %.2f cm\n", distance);
        if (distance <= 10.0)
        {
            stop_motor();
            break;
        }
        move_motor(pwm_l, pwm_r);
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Turn 90 degrees to the right
    turn_motor(1);
    vTaskDelay(pdMS_TO_TICKS(500));

    // Move forward 90cm
    moved_distance = 0.0;
    complete_movement = false;
    start_tracking(90.0); // Start tracking for 90cm

    while (!complete_movement)
    {
        move_motor(pwm_l, pwm_r);
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Stop the motor and end the task
    stop_motor();
    vTaskDelete(NULL);
<<<<<<< HEAD
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

void print_distance_task(void *param)
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

=======
}
>>>>>>> parent of ad286d0 (Revert back to debug ultrasonic)
