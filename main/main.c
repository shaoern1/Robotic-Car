#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h>
#include "wheels.h"
#include "ultrasonic.h"
#include "encoder.h"

#define BTN_START 21
#define ANGLE_TO_TURN 90

bool test_active = false;
/// @brief this function can be called for changing state (so I don't have to rewrite the code in different parts) 
void change_state(uint8_t next_state);
// Variables for tracking what step of the test we are at
enum STATION_1_STATE
{
    STATION_1_FIRST_PART = 0, // Moving until object found 10cm away
    STATION_1_TURN, // Turn 90 degrees to the right
    STATION_1_90_CM // Move forward 90cm in 5 seconds
};
uint8_t station1_state = STATION_1_FIRST_PART;

// Timer variables and functions to manage polling of devices
struct repeating_timer pid_timer;
struct repeating_timer ultrasonic_timer;
/// @brief checks the distance to the object in front of the car. If less than 10, stop 
bool ultrasonic_sensor_callback(struct repeating_timer *t);

// For turning task
// This is how much the left wheel needs to turn when 
float distToTurn = 0.f;

void change_state(uint8_t next_state)
{
    station1_state = next_state;
    test_active = false;
    set_wheels_duty_cycle(0.f);
    switch (next_state)
    {
        case STATION_1_FIRST_PART:
            // Move the car forward at max speed
            printf("Start works\n");		
            set_car_state(CAR_FORWARD);
            // Start timer
            add_repeating_timer_ms(1000, ultrasonic_sensor_callback, NULL, &ultrasonic_timer);
            break;
        case STATION_1_TURN:
            printf("Turn works\n");	
            station1_state = STATION_1_TURN;
            cancel_repeating_timer(&ultrasonic_timer);
            set_car_state(CAR_TURN_RIGHT);
            set_wheels_duty_cycle(0.f);
            distToTurn = (float)ANGLE_TO_TURN / 360.f;
            break;
        case STATION_1_90_CM:
            printf("90 cm works\n");
            // Move the car forward at max speed
            set_car_state(CAR_FORWARD);
            break;
    }
}

void init_gpio();
void init_interrupts();
void irq_handler(uint gpio, uint32_t events);

int main() 
{
    init_gpio();
    change_state(STATION_1_FIRST_PART);
    init_interrupts();

    while (true) 
        tight_loop_contents();
}

void init_gpio() 
{
    stdio_init_all();
    init_wheels();
    setupUltrasonicPins();
}
void init_interrupts()
{
    gpio_set_irq_enabled_with_callback(BTN_START, GPIO_IRQ_EDGE_FALL, true, &irq_handler);
    gpio_set_irq_enabled_with_callback(WHEEL_ENCODER_RIGHT_PIN, GPIO_IRQ_EDGE_FALL, true, &irq_handler);
    gpio_set_irq_enabled_with_callback(WHEEL_ENCODER_LEFT_PIN, GPIO_IRQ_EDGE_FALL, true, &irq_handler);
}
void irq_handler(uint gpio, uint32_t events)
{
    if (gpio == BTN_START)
    {
        if (!test_active)
        {
            set_wheels_duty_cycle(1.f);
            test_active = true;
        }
    }
    else if (gpio == WHEEL_ENCODER_LEFT_PIN && gpio == WHEEL_ENCODER_RIGHT_PIN)
    {
        encoderCallback(gpio, events);
        if (station1_state == STATION_1_TURN && leftTotalDistance >= distToTurn)
            change_state(STATION_1_90_CM);
        else if (station1_state == STATION_1_90_CM && leftTotalDistance >= 90)
            change_state(STATION_1_90_CM);
    }
}

bool ultrasonic_sensor_callback(struct repeating_timer *t)
{
    // Checks the current state of the test. If we are not at the first part of the test (Moving until object found 10cm away),
    // stop this timer (because we have no reason to check distance otherwise)
    if (station1_state == STATION_1_FIRST_PART)
    {
        
        float distance_to_item = getCm();
        printf("Distance to item: %.2f cm\n", distance_to_item);	
        if (distance_to_item <= 10.f && distance_to_item > 0.0f)
            change_state(STATION_1_TURN);
    }
    else 
    {
        // Realistically, we should never come here, but just to be safe...
        cancel_repeating_timer(&ultrasonic_timer);
    }
    return true;
}
