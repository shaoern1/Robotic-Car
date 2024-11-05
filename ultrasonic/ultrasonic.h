#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define TRIG_PIN 0
#define ECHO_PIN 1

typedef struct kalman_state_ kalman_state;
extern volatile bool obstacle_detected;

kalman_state *kalman_init(double q, double r, double p, double initial_value);
void echo_pulse_handler(uint gpio, uint32_t events);
void kalman_update(kalman_state *state, double measurement);
void ultrasonic_init();
uint64_t ultrasonic_get_pulse();
double ultrasonic_get_distance(kalman_state *state);

#endif
