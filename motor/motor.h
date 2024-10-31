#ifndef MOTOR_H
#define MOTOR_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// GPIO pins for motor 1
#define PWM_PIN1 4
#define DIR_PIN1_1 2
#define DIR_PIN1_2 3

// GPIO pins for motor 2
#define PWM_PIN2 6
#define DIR_PIN2_1 8
#define DIR_PIN2_2 9

// GPIO pins for buttons
#define BTN20 20
#define BTN21 21
#define BTN22 22

// Variables
extern float mtr1_speed;
extern float mtr2_speed;

// Function prototypes
void init_barcode();
void setup_gpio_pins();
void setup_pwm(uint gpio, float freq, float duty_cycle);

// Motor 1 control functions
void mtr1_fwd();
void mtr1_bwd();
void mtr1_stop();
void set_mtr1_speed(float speed);

// Motor 2 control functions
void mtr2_fwd();
void mtr2_bwd();
void mtr2_stop();
void set_mtr2_speed(float speed);

// Preset speeds
void set_speed_20();
void set_speed_40();
void set_speed_60();
void set_speed_80();
void set_speed_100();

// Preset movements
void pivot_left();
void pivot_right();
void lean_left();
void lean_right();
void move_fwd();
void move_bwd();
void stop();

#endif // MOTOR_H