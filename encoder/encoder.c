// Control L and R encoder

#include "encoder.h"

// Global variable declarations
volatile bool is_complete_movement = false;
uint32_t target_grid_number = 0;
uint32_t pulse_count_l = 0;
uint32_t pulse_count_r = 0;
volatile uint32_t pulse_count = 0;
volatile float moved_distance = 0.0f;  // Add this line
volatile float actual_speed_l;
volatile float actual_speed_r;
//static volatile float moved_distance = 0.0f;
// Function to get motor speed and distance
void get_speed_and_distance(int encoder, uint32_t pulse)
{
    // Calculate motor speed in cm/s
    double distance_per_hole = ENCODER_CIRCUMFERENCE / ENCODER_NOTCH;
    double distance = distance_per_hole * pulse;
    double speed = distance / 0.075;

    // Calculate and accumulate the distance
    moved_distance += distance;

    // Print motor speed and total distance
    if (encoder == 0)
    {
        actual_speed_l = speed;
    }
    else if (encoder == 1)
    {
        actual_speed_r = speed;
    }
    return;
}

void start_tracking(int target_grid)
{
    moved_distance = 0;              // Reset the distance moved
    target_grid_number = target_grid; // set the target number of grids
    is_complete_movement = false;
    return;
}

uint32_t get_grids_moved(bool reset)
{
    encoder_callback(); // Calculate final moved_distancce

    uint32_t grids_moved = (moved_distance / 14);
    printf("DISTANCE TRAVELLED: %.2lf\n", moved_distance);

    if (reset)
    {
        // Reset the distance moved
        moved_distance = 0.0;
    }

    return grids_moved;
}

// Function to count each encoder's pulse
void encoder_pulse(uint gpio)
{
    // Increment pulse counts based on GPIO pin
    if (gpio == L_ENCODER_OUT)
    {
        pulse_count_l++;
    }
    else if (gpio == R_ENCODER_OUT)
    {
        pulse_count_r++;
    }

    // Update moved distance
    float distance_per_pulse = WHEEL_CIRCUMFERENCE / ENCODER_NOTCH;
    moved_distance += distance_per_pulse;
}

// Function to interrupt every second
bool encoder_callback()
{
    // Call get_speed_and_distance function every second
    get_speed_and_distance(0, pulse_count_l);
    get_speed_and_distance(1, pulse_count_r);

    // Reset the pulse counts
    pulse_count_l = 0;
    pulse_count_r = 0;

    if (target_grid_number > 0)
    {
        uint32_t grids_moved = (moved_distance / 10.5);
        if (grids_moved >= target_grid_number)
        {
            target_grid_number = 0;    // Reset target number of grids
            moved_distance = 0;       // Reset moved distance
            is_complete_movement = true; // Set flag to indicate target number of grids reached
        }
    }
    return true;
}

// Function to initialize pins for encoders
void init_encoder_setup()
{
    // Initialize GPIO pins for L encoder
    gpio_init(L_ENCODER_POW);
    gpio_init(L_ENCODER_OUT);

    // Set GPIO pins as outputs for L encoder
    gpio_set_dir(L_ENCODER_POW, GPIO_OUT);
    gpio_set_dir(L_ENCODER_OUT, GPIO_IN);

    // Set GPIO settings for L encoder
    gpio_pull_up(L_ENCODER_OUT);
    // gpio_set_irq_enabled_with_callback(L_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &encoder_pulse);

    // Initialize GPIO pins for R encoder
    gpio_init(R_ENCODER_POW);
    gpio_init(R_ENCODER_OUT);

    // Set GPIO pins as outputs for R encoder
    gpio_set_dir(R_ENCODER_POW, GPIO_OUT);
    gpio_set_dir(R_ENCODER_OUT, GPIO_IN);

    // Set GPIO settings for R encoder
    gpio_pull_up(R_ENCODER_OUT);

    // Enable the POW pins
    gpio_put(L_ENCODER_POW, 1);
    gpio_put(R_ENCODER_POW, 1);
}



void reset_encoder_counts()
{
    moved_distance = 0.0f;
    // Reset encoder pulse counts as well
    pulse_count_l = 0;
    pulse_count_r = 0;
}

float get_moved_distance()
{
    return moved_distance;
}