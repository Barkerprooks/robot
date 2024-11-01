#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "sixaxis.h"

#include <stdint.h>

#define I2C_SDA_PIN 4
#define I2C_SCK_PIN 5

#define MPU6050_DEVICE_ID 0x68
#define MPU6050_ACCEL_REG 0x3b
#define MPU6050_GYRO_REG 0x43

static void bytes_to_tuple(uint8_t buffer[6], struct tuple *data) {
    data->x = (buffer[0] << 8 | buffer[1]);
    data->y = (buffer[2] << 8 | buffer[3]);
    data->z = (buffer[4] << 8 | buffer[5]);
}

void init_sixaxis() {
    i2c_init(i2c0, 400000); // 400 KHz

    // set up the TX pin and clock pin to pull up by default
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);   
    gpio_set_function(I2C_SCK_PIN, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCK_PIN);
}

bool read_sixaxis(struct sixaxis *data) {
    uint8_t buffer[6]; // 6 bytes = 3 shorts (x, y, z)

    // request a read from the accelerometer
    uint8_t accel_reg = MPU6050_ACCEL_REG;
    uint8_t gyro_reg = MPU6050_GYRO_REG;

    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, &accel_reg, 1, true);
    i2c_read_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 6, false);

    // dump the buffer data into the x, y, z fields
    bytes_to_tuple(buffer, &(data->accel));

    // repeat the same operation but with the gyro register
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, &gyro_reg, 1, true);
    i2c_read_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 6, false);

    // dump gyro buffer
    bytes_to_tuple(buffer, &(data->gyro));
}

void reset_sixaxis() {
    // 0x6b = POWER_CNTL, 0x00 = power on / reset
    uint8_t reset_bytes[2] = { 0x6b, 0x00 };
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, reset_bytes, 2, false);
}