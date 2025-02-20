// Control L and R encoder

#include "encoder.h"
#include "motor.h"
// Global variable declaration
volatile bool complete_movement = false;
uint32_t target_grid_number = 0;
uint32_t pulse_count_l = 0;
uint32_t pulse_count_r = 0;
volatile uint32_t oscillation = 0;
double moved_distance = 0.0;
volatile float actual_speed_l;
volatile float actual_speed_r;

// Function to get motor speed and distance
void get_speed_and_distance(int encoder, uint32_t pulse_count)
{
    // Calculate motor speed in cm/s
    double distance_per_hole = ENCODER_CIRCUMFERENCE / ENCODER_NOTCH;
    double distance = distance_per_hole * pulse_count;
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

void start_tracking(int target_grids)
{
    moved_distance = 0;              // Reset the distance moved
    target_grid_number = target_grids; // set the target number of grids
    complete_movement = false;
    printf("TARGET GRIDS: %d\n", target_grid_number);
    return;
}

uint32_t get_grids_moved(bool reset)
{
    encoder_callback(); // Calculate final movedDistance

    uint32_t grids_moved = moved_distance / 14;
    printf("DISTANCE TRAVELLED: %.2lf\n", moved_distance);

    if (reset)
    {
        // Reset the distance moved
        moved_distance = 0.0;
    }

    return grids_moved;
}

// Function to count each encoder's pulse
void encoder_pulse(uint gpio, uint32_t events)
{
    if (gpio == L_ENCODER_OUT)
    {
        pulse_count_l++;
    }
    else if (gpio == R_ENCODER_OUT)
    {
        pulse_count_r++;
    }

    oscillation++;
}

#define TARGET_DISTANCE_CM 90.0  // Define target distance in centimeters

// Function to interrupt every second
bool encoder_callback()
{
    // Call get_speed_and_distance function every second
    get_speed_and_distance(0, pulse_count_l);
    get_speed_and_distance(1, pulse_count_r);

    pulse_count_l = 0;
    pulse_count_r = 0;

    if (moved_distance >= TARGET_DISTANCE_CM)
    {
        complete_movement = true;
        stop_motor();
        printf("Target distance of %.2f cm reached. Stopping motor.\n", TARGET_DISTANCE_CM);
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