/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include <math.h>

// WIFI Configuration

#define TCP_PORT 4242
#define SERVER_IP "172.20.10.2"  // IP address of the server EDIT THIS !!! (use ipconfig on the server to get the IP address)


// I2C Configuration
#define I2C_PORT i2c0
#define SDA_PIN 4   // Changed from 0 to 4
#define SCL_PIN 5   // Changed from 1 to 5
#define ACCELEROMETER_ADDR 0x19
#define CTRL_REG1_A 0x20
#define CTRL_REG4_A 0x23
#define OUT_X_L_A 0x28

// The LSM303DLHC in ±4g range has a sensitivity of 2mg/LSB (LSB = Least Significant Bit)
// 1g = 1000mg = 500 LSB
#define SENSITIVITY_4G 2.0f/1000.0f  // Convert raw to g (±4g range)
#define TILT_THRESHOLD 3.6f          // Trigger tilt at 3.6g
#define SAMPLE_RATE 500              // ms between readings

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

static struct tcp_pcb *tcp_client_pcb;
static struct tcp_pcb *connection_pcb = NULL;

static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err) {
    printf("Client: Connected to server\n");
    connection_pcb = pcb;
    return ERR_OK;
}

void main_task(__unused void *params) {
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        exit(1);
    } else {
        printf("Connected to wifi.\n");
    }

    tcp_client_pcb = tcp_new();
    if (!tcp_client_pcb) {
        printf("Client: Error creating PCB.\n");
        return;
    }


    ip_addr_t server_ip;
    ip4addr_aton(SERVER_IP, &server_ip);

    tcp_connect(tcp_client_pcb, &server_ip, TCP_PORT, tcp_client_connected);
    
    while(true) {
        vTaskDelay(100);
    }

    cyw43_arch_deinit();
}

void acc_init() {
    uint8_t config_data[2];

    // Set CTRL_REG1_A: 100Hz data rate, XYZ axes enabled
    config_data[0] = CTRL_REG1_A;
    config_data[1] = 0x57;
    i2c_write_blocking(I2C_PORT, ACCELEROMETER_ADDR, config_data, 2, false);

    // Set CTRL_REG4_A: ±2g range
    config_data[0] = CTRL_REG4_A;
    config_data[1] = 0x10;
    i2c_write_blocking(I2C_PORT, ACCELEROMETER_ADDR, config_data, 2, false);
}

// Function to read and convert accelerometer data to g-forces
void acc_read_g(float *ax_g, float *ay_g, float *az_g) {
    uint8_t reg = OUT_X_L_A | 0x80;  // Auto-increment
    uint8_t buf[6];
    int16_t ax, ay, az;

    i2c_write_blocking(I2C_PORT, ACCELEROMETER_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, ACCELEROMETER_ADDR, buf, 6, false);

    // Convert to 16-bit integers
    ax = (int16_t)((buf[1] << 8) | buf[0]);
    ay = (int16_t)((buf[3] << 8) | buf[2]);
    az = (int16_t)((buf[5] << 8) | buf[4]);

    // Convert to g-forces
    *ax_g = ax * SENSITIVITY_4G;
    *ay_g = ay * SENSITIVITY_4G;
    *az_g = az * SENSITIVITY_4G;
}

// Function to calculate tilt angles in degrees
void calculate_tilt_angles(float ax_g, float ay_g, float az_g, float *pitch, float *roll) {
    // Calculate pitch (rotation around Y-axis) and roll (rotation around X-axis)
    *pitch = atan2f(ax_g, sqrtf(ay_g * ay_g + az_g * az_g)) * 180.0f / M_PI;
    *roll = atan2f(ay_g, sqrtf(ax_g * ax_g + az_g * az_g)) * 180.0f / M_PI;
}

void send_data(char direction[10], int speed) {
    if (connection_pcb == NULL) {
        printf("Client: Not connected to server.\n");
        return;
    }

    char msg[50];
    snprintf(msg, sizeof(msg), "Direction: %s, Speed: %d\n", direction, speed);

    if (tcp_write(connection_pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY) != ERR_OK) {
        printf("Client: Error sending data.\n");
    } else {
        tcp_output(connection_pcb);
        printf("Client: Sent data: %s\n", msg);
    }
}

void print_direction(float ax_g, float ay_g, float pitch, float roll) {
    // Print direction based on g-forces
    char direction[10];
    int pitch_int = (int) pitch;
    int roll_int = (int) roll;
    if (ax_g < -TILT_THRESHOLD) {
        // printf("Forward at %d speed", abs(pitch_int));
        strcpy(direction, "Forward");
        send_data(direction, abs(pitch_int));
    } else if (ax_g > TILT_THRESHOLD) {
        // printf("Backward at %d speed", abs(pitch_int));
        strcpy(direction, "Backward");
        send_data(direction, abs(pitch_int));
    }

    if (ay_g < -TILT_THRESHOLD) {
        // printf("Left at %d speed", abs(roll_int));
        strcpy(direction, "Left");
        send_data(direction, abs(roll_int));
    } else if (ay_g > TILT_THRESHOLD) {
        // printf("Right at %d speed", abs(roll_int));
        strcpy(direction, "Right");
        send_data(direction, abs(roll_int));
    }

    if (fabsf(ax_g) < TILT_THRESHOLD && fabsf(ay_g) < TILT_THRESHOLD) {
        // printf("Neutral");
        strcpy(direction, "Neutral");
        send_data(direction, 0);
    }
    
    printf("\n");
}



void accelerometer_task(__unused void *params) {

 // Initialize I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Initialize accelerometer
    acc_init();
    sleep_ms(500);

    float ax_g, ay_g, az_g;
    float pitch, roll;

    while (1) {
        // Read accelerometer (in g-forces)
        acc_read_g(&ax_g, &ay_g, &az_g);
        
        // Calculate tilt angles
        calculate_tilt_angles(ax_g, ay_g, az_g, &pitch, &roll);
        
        // Print values and direction
        // printf("G-forces - X: %.2f g, Y: %.2f g, Z: %.2f g\n", ax_g, ay_g, az_g);
        // printf("Angles - Pitch: %.1f°, Roll: %.1f°\n", pitch, roll);
        // printf("Direction: ");
        print_direction(ax_g, ay_g, pitch, roll);
        // printf("\n");
        
        sleep_ms(SAMPLE_RATE);
    }


    // while (1) {
    //     vTaskDelay(500);
    //     char msg[50];
    //     acc_read_raw(&ax, &ay, &az);
    //     snprintf(msg, sizeof(msg), "Accelerometer Raw Data - X: %d, Y: %d, Z: %d\n", ax, ay, az);
    //     printf("Accelerometer Raw Data - X: %d, Y: %d, Z: %d\n", ax, ay, az);


    //     if (tcp_write(connection_pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY) != ERR_OK) {
    //         printf("\nClient: Error sending magnetometer data.\n");
    //     } else {
    //         tcp_output(connection_pcb);
    //         printf("\nClient: Sent data: %s\n", msg);
    //     }
    // }
}


void vLaunch( void) {
    TaskHandle_t task;
    xTaskCreate(main_task, "MainThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &task);

    TaskHandle_t accelerometertask;
    xTaskCreate(accelerometer_task, "MagnetometerThread", configMINIMAL_STACK_SIZE, NULL, 8, &accelerometertask);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
    // takes care of it otherwise)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main( void )
{
    stdio_init_all();

    /* Configure the hardware ready to run the demo. */
    const char *rtos_name;
#if ( portSUPPORT_SMP == 1 )
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
    printf("Starting %s on both cores:\n", rtos_name);
    vLaunch();
#elif ( RUN_FREERTOS_ON_CORE == 1 )
    printf("Starting %s on core 1:\n", rtos_name);
    multicore_launch_core1(vLaunch);
    while (true);
#else
    printf("Starting %s on core 0:\n", rtos_name);
    vLaunch();
#endif
    return 0;
}
