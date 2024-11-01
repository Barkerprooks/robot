#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include <stdint.h>
#include <math.h>

#include "sixaxis.h"

#define I2C_SDA_PIN 4
#define I2C_SCK_PIN 5

#define MPU6050_DEVICE_ID 0x68

#define MPU6050_POWER_REG 0x6b
#define MPU6050_ACCEL_REG 0x3b
#define MPU6050_GYRO_REG  0x43

#define ACCEL_SCALE 16384.0
#define GYRO_SCALE 131.0

static void bytes_to_tuple(uint8_t buffer[6], struct tuple *data) {
    data->x = (buffer[0] << 8 | buffer[1]);
    data->y = (buffer[2] << 8 | buffer[3]);
    data->z = (buffer[4] << 8 | buffer[5]);
}

void sixaxis_init() {
    uint8_t power_ctrl[2] = { MPU6050_POWER_REG, 0x00 }; // 0x00 = request for power on

    i2c_init(i2c0, 400000); // 400 KHz clock speed

    // set up the TX pin and clock pin to pull up by default
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);   
    gpio_set_function(I2C_SCK_PIN, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCK_PIN);

    // request for the device to power itself up
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, power_ctrl, 2, false);
}

void sixaxis_read(struct sixaxis *data) {
    uint8_t device[1] = { MPU6050_ACCEL_REG }; // buffer to send the request for which device we want
    uint8_t buffer[6]; // 6 bytes = 3 shorts (x, y, z)

    // request a read from the accelerometer
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, device, 1, true);
    i2c_read_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 6, false);

    // dump the buffer data into the x, y, z fields
    bytes_to_tuple(buffer, &(data->accel));

    device[0] = MPU6050_GYRO_REG; // switch to the gyro register and repeat the process

    // repeat the same operation but with the gyro register
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, device, 1, true);
    i2c_read_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 6, false);

    // dump gyro buffer
    bytes_to_tuple(buffer, &(data->gyro));
}

float angle(struct sixaxis data, float delta) {
    float denominator = sqrt(pow(data.accel.x / ACCEL_SCALE, 2) + pow(data.accel.z / ACCEL_SCALE, 2));
    float accel_angle = atan((data.accel.y / ACCEL_SCALE) / denominator) * 57.29577950560105;
    float gyro_angle = data.gyro.x / 131.0;
    return 0.98 * (gyro_angle * delta) + 0.02 * accel_angle;
}