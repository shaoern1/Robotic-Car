#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

// I2C Configuration
#define I2C_PORT i2c0
#define SDA_PIN 4   // Changed from 0 to 4
#define SCL_PIN 5   // Changed from 1 to 5
#define MAGNETOMETER_ADDR 0x1E
#define ACCELEROMETER_ADDR 0x19
#define CTRL_REG1_A 0x20
#define CTRL_REG4_A 0x23
#define OUT_X_L_A 0x28

// Function to initialize the MAGNETOMETER sensor
void mag_init() {
    uint8_t config_data[2];

    // Configuration Register A: 8-average, 15 Hz default, normal measurement
    config_data[0] = 0x00; // Register address
    config_data[1] = 0x70; // 0b01110000
    // writes data to I2C device -> port, address of sensor, data to write(register add & value), num of bytes, repeat
    i2c_write_blocking(I2C_PORT, MAGNETOMETER_ADDR, config_data, 2, false);

    // Configuration Register B: Gain = 1.3 Ga
    config_data[0] = 0x01; // Register address
    config_data[1] = 0x20; // 0b00100000
    i2c_write_blocking(I2C_PORT, MAGNETOMETER_ADDR, config_data, 2, false);

    // Mode Register: Continuous measurement mode
    config_data[0] = 0x02; // Register address
    config_data[1] = 0x00; // 0b00000000
    i2c_write_blocking(I2C_PORT, MAGNETOMETER_ADDR, config_data, 2, false);
}

// Function to read raw magnetometer data
void mag_read_raw(int16_t *mx, int16_t *my, int16_t *mz) {
    uint8_t reg = 0x03; // Data Output X MSB Register
    uint8_t buf[6];

    // Start reading from Data Output X MSB Register
    i2c_write_blocking(I2C_PORT, MAGNETOMETER_ADDR, &reg, 1, true);
    // read 6 bytes of data into buf
    i2c_read_blocking(I2C_PORT, MAGNETOMETER_ADDR, buf, 6, false);

    // Combine MSB and LSB for each axis
    *mx = (buf[0] << 8) | buf[1]; // X-axis
    *mz = (buf[2] << 8) | buf[3]; // Z-axis
    *my = (buf[4] << 8) | buf[5]; // Y-axis
}

// Function to initialize the accelerometer
void acc_init() {
    uint8_t config_data[2];

    // Set CTRL_REG1_A to enable the accelerometer (100Hz ODR, all axes enabled)
    config_data[0] = CTRL_REG1_A;   // CTRL_REG1_A register address
    config_data[1] = 0x57;          // 100Hz data rate, XYZ axes enabled (01010111)
    i2c_write_blocking(I2C_PORT, ACCELEROMETER_ADDR, config_data, 2, false);

    // Set CTRL_REG4_A to set +/-16g full-scale
    config_data[0] = CTRL_REG4_A;   // CTRL_REG4_A register address
    config_data[1] = 0x00;          // +/-2g full scale, high-resolution mode (00000000)
    i2c_write_blocking(I2C_PORT, ACCELEROMETER_ADDR, config_data, 2, false);
}

// Function to read raw accelerometer data
void acc_read_raw(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t reg = OUT_X_L_A | 0x80;  // OUT_X_L_A register address with auto-increment
    uint8_t buf[6];

    // Read 6 bytes of data starting from OUT_X_L_A (auto-increment)
    i2c_write_blocking(I2C_PORT, ACCELEROMETER_ADDR, &reg, 1, true);  // Set the starting register
    i2c_read_blocking(I2C_PORT, ACCELEROMETER_ADDR, buf, 6, false);   // Read 6 bytes of data

    // Combine MSB and LSB for each axis (2's complement values)
    *ax = (int16_t)((buf[1] << 8) | buf[0]); // X-axis
    *ay = (int16_t)((buf[3] << 8) | buf[2]); // Y-axis
    *az = (int16_t)((buf[5] << 8) | buf[4]); // Z-axis
}

int main() {
    stdio_init_all();

    // Initialize I2C at 400 kHz
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);


    // Initialize the MAGNETOMETER sensor
    // mag_init();
    // sleep_ms(500);

    // int16_t mx, my, mz;

    // while (1) {
    //     mag_read_raw(&mx, &my, &mz);
    //     printf("Magnetometer Raw Data - X: %d, Y: %d, Z: %d\n", mx, my, mz);
    //     sleep_ms(500);
    // }

    acc_init();
    sleep_ms(500);
    int16_t ax, ay, az;

    while (1) {
        acc_read_raw(&ax, &ay, &az);
        printf("Accelerometer Raw Data - X: %d, Y: %d, Z: %d\n", ax, ay, az);
        sleep_ms(500);
    }

    return 0;
}