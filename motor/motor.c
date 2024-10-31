##include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "motor.h"
#include "math.h"


// Define variables
float mtr1_speed = 0.0f;
float mtr2_speed = 0.0f;

void setup_gpio_pins() {
    // Motor 1
    gpio_init(DIR_PIN1_1);
    gpio_init(DIR_PIN1_2);
    gpio_set_dir(DIR_PIN1_1, GPIO_OUT);
    gpio_set_dir(DIR_PIN1_2, GPIO_OUT);

    // Motor 2
    gpio_init(DIR_PIN2_1);
    gpio_init(DIR_PIN2_2);
    gpio_set_dir(DIR_PIN2_1, GPIO_OUT);
    gpio_set_dir(DIR_PIN2_2, GPIO_OUT);

    // Set up PWM for mtr1 n mtr2
    setup_pwm(PWM_PIN1, 100.0f, mtr1_speed);
    setup_pwm(PWM_PIN2, 100.0f, mtr2_speed);
}

void init_barcode() {
    setup_gpio_pins();
    setup_pwm(PWM_PIN1, 100.0f, mtr1_speed);
    setup_pwm(PWM_PIN2, 100.0f, mtr2_speed);
}
void setup_pwm(uint gpio, float freq, float duty_cycle) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    float clock_freq = 125000000.0f;
    uint32_t divider = clock_freq / (freq * 65535.0f);
    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_gpio_level(gpio, (uint16_t)(duty_cycle * 65536.0f));
    pwm_set_enabled(slice_num, true);
}

// Motor 1 Functions
void mtr1_fwd() {
    gpio_put(DIR_PIN1_1, 1);
    gpio_put(DIR_PIN1_2, 0);
    printf("Motor 1: Forward\n");
}

void mtr1_bwd() {
    gpio_put(DIR_PIN1_1, 0);
    gpio_put(DIR_PIN1_2, 1);
    printf("Motor 1: Backward\n");
}

void mtr1_stop() {
    gpio_put(DIR_PIN1_1, 0);
    gpio_put(DIR_PIN1_2, 0);
    printf("Motor 1: Stop\n");
}

void set_mtr1_speed(float speed) {
    pwm_set_gpio_level(PWM_PIN1, (uint16_t)(speed * 65536.0f));
}

// Motor 2 Functions
void mtr2_fwd() {
    gpio_put(DIR_PIN2_1, 1);
    gpio_put(DIR_PIN2_2, 0);
    printf("Motor 2: Forward\n");
}

void mtr2_bwd() {
    gpio_put(DIR_PIN2_1, 0);
    gpio_put(DIR_PIN2_2, 1);
    printf("Motor 2: Backward\n");
}

void mtr2_stop() {
    gpio_put(DIR_PIN2_1, 0);
    gpio_put(DIR_PIN2_2, 0);
    printf("Motor 2: Stop\n");
}

void set_mtr2_speed(float speed) {
    pwm_set_gpio_level(PWM_PIN2, (uint16_t)(speed * 65536.0f));
    float spd_prct = speed * 100.0f;
    printf("Motor 2: Speed: %f\n", spd_prct);
}

// Preset Speeds
void set_speed_20() {
    set_mtr1_speed(0.2f);
    set_mtr2_speed(0.2f);
}

void set_speed_40() {
    set_mtr1_speed(0.4f);
    set_mtr2_speed(0.4f);
}

void set_speed_60() {
    set_mtr1_speed(0.6f);
    set_mtr2_speed(0.6f);
}

void set_speed_80() {
    set_mtr1_speed(0.8f);
    set_mtr2_speed(0.8f);
}

void set_speed_100() {
    set_mtr1_speed(1.0f);
    set_mtr2_speed(1.0f);
}

// Preset Movements
// Pivots
void pivot_left() {
    mtr1_fwd();
    mtr2_bwd();
    set_mtr1_speed(0.5f);
    set_mtr2_speed(0.5f);
}

void pivot_right() {
    mtr1_bwd();
    mtr2_fwd();
    set_mtr1_speed(0.5f);
    set_mtr2_speed(0.5f);
}

// Leaning
void lean_left() {
    mtr1_fwd();
    mtr2_fwd();
    set_mtr1_speed(0.4f);
    set_mtr2_speed(0.6f);
}

void lean_right() {
    mtr1_fwd();
    mtr2_fwd();
    set_mtr1_speed(0.6f);
    set_mtr2_speed(0.4f);
}

// Forward and Backward
void move_fwd() {
    mtr1_fwd();
    mtr2_fwd();
    set_mtr1_speed(0.5f);
    set_mtr2_speed(0.5f);
}

void move_bwd() {
    mtr1_bwd();
    mtr2_bwd();
    set_mtr1_speed(0.5f);
    set_mtr2_speed(0.5f);
}

void stop() {
    mtr1_stop();
    mtr2_stop();
    set_mtr1_speed(0.0f);
    set_mtr2_speed(0.0f);
}


