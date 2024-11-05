#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "motor.h"
#include "encoder.h"
#include "ultrasonic.h"

#define BUTTON_PIN 21      // GPIO pin connected to the button

typedef enum {
    IDLE = 0,
    Ultrasonic10cm,
    Turn90Degrees,
    Travel90cm
} RobotState;

// Function prototypes
void init_interrupt();
void ihandler(uint gpio, uint32_t events);
void turn_right_90();
void move_forward_cm(float distance);
void change_state(); // Ensure this matches the definition

// Global variables
RobotState current_state = IDLE;
kalman_state *ultrasonic_state;

int main()
{
    // Initialize standard I/O  
    stdio_init_all();
    init_motor_setup();
    init_motor_pwm();
    init_encoder_setup();
    ultrasonic_init();
    ultrasonic_state = kalman_init(1, 100, 1, 0);
    
    // Initialize interrupt
    init_interrupt();
    
    // Initialize Button GPIO
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Use pull-up resistor
    
    printf("Press the button to start.\n");
    
    // Wait for button press (active low)
    while (gpio_get(BUTTON_PIN)) {
        sleep_ms(100);
    }
    
    printf("Button pressed. Starting program.\n");
    current_state = Ultrasonic10cm;
    
    while (1)
    {
        change_state();
        sleep_ms(50);
    }
}

void change_state(){
    printf("Current state: %d\n", current_state);
    switch (current_state)
    {   
        case IDLE:
        {
            printf("IDLE\n");
            stop_motor();
            break;
        }
        case Ultrasonic10cm:
        {
            double distance = ultrasonic_get_distance(ultrasonic_state);
            printf("Distance: %.2lf cm\n", distance);
            if (distance > 10.0)
            {
                move_motor(pwm_l, pwm_r); // Move forward
            }
            else
            {
                printf("Obstacle detected. Stopping motor.\n");
                stop_motor();
                current_state = Turn90Degrees;
            }
            break;
        }
        case Turn90Degrees:
        {
            printf("Turning 90 degrees\n");
            turn_right_90();
            current_state = Travel90cm;
            break;
        }
        case Travel90cm:
        {
            printf("Traveling 90 cm\n");
            move_forward_cm(90.0); // Move forward 90 cm
            current_state = Ultrasonic10cm;
            break;
        }
        default:
            break;
    }
}

// Initialize interrupt for encoder and ultrasonic sensor
void init_interrupt(){
    gpio_set_irq_enabled_with_callback(L_ENCODER_OUT, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &ihandler);
    gpio_set_irq_enabled_with_callback(R_ENCODER_OUT, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &ihandler);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &ihandler);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &ihandler);
}

void ihandler(uint gpio, uint32_t events){
    if (gpio == BUTTON_PIN)
    {
        if (current_state == IDLE)
            current_state = Ultrasonic10cm;         
        else if (current_state == Ultrasonic10cm)
            current_state = Turn90Degrees;
        else if (current_state == Turn90Degrees)
            current_state = Travel90cm;
    }
    // Encoder interrupt handler
    if (gpio == L_ENCODER_OUT || gpio == R_ENCODER_OUT){
        encoder_pulse(gpio, events);
    }
    // Ultrasonic sensor interrupt handler
    else if (gpio == ECHO_PIN){
        echo_pulse_handler(gpio, events);
    }
}

// Function to turn right 90 degrees
void turn_right_90()
{

    // Set PWM for turning
    pwm_l = 3125;
    pwm_r = 3125;
    turn_motor(1);

    // Wait for turn to complete (adjust delay as needed)
    sleep_ms(500);

    stop_motor();
}

// Function to move forward a specific distance in cm
void move_forward_cm(float distance)
{
    printf("Moving forward %.2f cm\n", distance);
    start_tracking(distance / 14.0); // Assuming 14 cm per grid
    // If the distance is 90 the car wil move 6 grids and once it met the car will stop motor
    while (!complete_movement)
    {
        printf("Moving forward\n");
        move_motor(pwm_l, pwm_r);
        sleep_ms(50);
        if (TARGET_DISTANCE_CM == distance)
        {
            stop_motor();
        }
    }
    printf("Target distance of %.2f cm reached. Stopping motor.\n", distance);
    stop_motor();

}