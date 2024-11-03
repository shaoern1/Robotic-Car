#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "motor.h"
#include "encoder.h"

#define BUTTON_PIN 21  // Replace with the GPIO pin connected to your button

void motor_task(void *pvParameters)
{
    // Initialize peripherals
    init_motor();
    init_encoder_setup();

    // Configure button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Assuming active-low button

    bool motor_running = false;
    bool button_pressed = false;
    float target_distance = 90.0f;  // Target distance in cm

    while (1)
    {
        // Check if button is pressed
        if (!gpio_get(BUTTON_PIN) && !button_pressed)
        {
            button_pressed = true;
            motor_running = !motor_running;

            if (motor_running)
            {
                // Reset encoder counts
                reset_encoder_counts();
                // Start motor
                printf("Starting motor...\n");
                move_fwd();
                set_speed_40();  // Set desired speed
            }
            else
            {
                // Stop motor
                stop();
            }
        }
        else if (gpio_get(BUTTON_PIN))
        {
            button_pressed = false;
        }

        // If motor is running, check distance
        if (motor_running)
        {
            // Update distance moved
            float distance = get_moved_distance();

            if (distance >= target_distance)
            {
                // Stop motor
                printf("Target distance reached. Stopping motor...\n");
                stop();
                motor_running = false;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));  // Delay for 10 ms
    }
}

int main()
{
    stdio_init_all();
    cyw43_arch_init();

    // Create motor task
    xTaskCreate(motor_task, "MotorTask", 1024, NULL, 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    while(1)
    {
        // Should never reach here
    }
}