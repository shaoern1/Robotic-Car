// Get readings from ultrasonic sensor

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "ultrasonic.h"

volatile absolute_time_t start_time;
volatile uint64_t pulse_width = 0;
volatile bool obstacle_detected = false;

typedef struct kalman_state_
{
    double q; // process noise covariance
    double r; // measurement noise covariance
    double x; // estimated value
    double p; // estimation error covariance
    double k; // kalman gain
} kalman_state;

kalman_state *kalman_init(double q, double r, double p, double initial_value)
{
    kalman_state *state = calloc(1, sizeof(kalman_state));
    state->q = q;
    state->r = r;
    state->p = p;
    state->x = initial_value;

    return state;
}

void echo_pulse_handler(uint gpio, uint32_t events)
{
    if (gpio == ECHO_PIN && events & GPIO_IRQ_EDGE_RISE)
    {
        // Rising edge detected, start the timer
        start_time = get_absolute_time();
    }
    else if (gpio == ECHO_PIN && events & GPIO_IRQ_EDGE_FALL)
    {
        // Falling edge detected, calculate the pulse width
        pulse_width = absolute_time_diff_us(start_time, get_absolute_time());
    }
}

void kalman_update(kalman_state *state, double measurement)
{
    // Prediction update
    state->p = state->p + state->q;

    // Measurement update
    state->k = state->p / (state->p + state->r);
    state->x = state->x + state->k * (measurement - state->x);
    state->p = (1 - state->k) * state->p;
}

void ultrasonic_init()
{
    gpio_init(TRIG_PIN);
    gpio_init(ECHO_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_pulse_handler);
}

uint64_t ultrasonic_get_pulse()
{
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);
    sleep_ms(1);

    return pulse_width;
}

double ultrasonic_get_distance(kalman_state *state)
{
    uint64_t pulse_length = ultrasonic_get_pulse();
    double measured = pulse_length / 29.0 / 2.0;
    kalman_update(state, measured);

    if (state->x < 10)
    {
        obstacle_detected = true;
    }

    return state->x;
}

/*
int main()
{
    // Driver code to run ultrasonic sensor
    double distance;
    stdio_init_all();
    printf("Initializing ultrasonic sensor\n");
    ultrasonic_init();
    kalman_state *state = kalman_init(1, 100, 0, 0);
    sleep_ms(1000);
    while (true)
    {
        for (int i = 0; i < 20; i++)
        {
            distance = ultrasonic_get_distance(state);
        }
        printf("Distance: %.2lf cm\n", distance);
        sleep_ms(500);
    }
}
*/
