#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Define encoder pins
#define L_ENCODER_POW 10 // GPIO pin for L encoder power
#define L_ENCODER_OUT 5  // GPIO pin for L encoder output
#define R_ENCODER_POW 17 // GPIO pin for R encoder power
#define R_ENCODER_OUT 7  // GPIO pin for R encoder output

// Define encoder disk specs
#define ENCODER_NOTCH 20        // in cm
#define ENCODER_CIRCUMFERENCE 8.5 // in cm
#define WHEEL_CIRCUMFERENCE 22  // in cm
#define TARGET_DISTANCE_CM 90.0 

// External variables
extern volatile bool complete_movement;
extern volatile uint32_t oscillation;
extern volatile float actual_speed_l;
extern volatile float actual_speed_r;
extern double moved_distance;


// Functions for encoders
void get_speed_and_distance(int encoder, uint32_t pulse_count);
void encoder_pulse(uint gpio, uint32_t events);
bool encoder_callback();
void init_encoder_setup();
uint32_t get_grids_moved(bool reset);
void start_tracking(int target_grids);

#endif