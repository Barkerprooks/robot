#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "sixaxis.h"

#define I2C_SDA_PIN 4
#define I2C_SCK_PIN 5

// address we're communicating with over i2c
#define MPU6050_DEVICE_ID 0x68

// power control address
#define MPU6050_POWER_REG 0x6b

#define MPU6050_GYRO_VALUES_REG 0x43
#define MPU6050_GYRO_CONFIG_REG 0X1b

#define MPU6050_ACCEL_VALUES_REG 0x3b
#define MPU6050_ACCEL_CONFIG_REG 0X1c

// sanity check register
#define MPU6050_WHO_AM_I_REG 0x75

// power settings
#define MPU6050_POWER_CLOCK_INTERNAL 0
#define MPU6050_POWER_CLOCK_XGYRO    1
#define MPU6050_POWER_CLOCK_YGYRO    2
#define MPU6050_POWER_CLOCK_ZGYRO    3
#define MPU6050_POWER_CLOCK_RESET    7

// gyro settings
#define MPU6050_GYRO_FREQ_250  0
#define MPU6050_GYRO_FREQ_500  1
#define MPU6050_GYRO_FREQ_1000 2
#define MPU6050_GYRO_FREQ_2000 3

// accelerometer settings
#define MPU6050_ACCEL_FREQ_2  0
#define MPU6050_ACCEL_FREQ_4  1
#define MPU6050_ACCEL_FREQ_8  2
#define MPU6050_ACCEL_FREQ_16 3

#define MPU6050_G 32768.0

static void bytes_to_tuple(uint8_t buffer[14], struct sixaxis *sensor) {
    sensor->accel.x = (buffer[0] << 8 | buffer[1]);
    sensor->accel.y = (buffer[2] << 8 | buffer[3]);
    sensor->accel.z = (buffer[4] << 8 | buffer[5]);
    sensor->gyro.x = (buffer[8] << 8 | buffer[9]);
    sensor->gyro.y = (buffer[10] << 8 | buffer[11]);
    sensor->gyro.z = (buffer[12] << 8 | buffer[13]);
}

static uint8_t sixaxis_test() {
    uint8_t buffer = MPU6050_WHO_AM_I_REG;

    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, &buffer, 1, true);
    i2c_read_blocking(i2c0, MPU6050_DEVICE_ID, &buffer, 1, false);

    return buffer == MPU6050_DEVICE_ID ? 0 : 1;
}

static void sixaxis_init_gyro(struct sixaxis_device *gyro, uint8_t frequency) {
    uint8_t buffer[2] = { MPU6050_GYRO_CONFIG_REG, frequency };
    switch (frequency) {
        case MPU6050_GYRO_FREQ_2000:
            gyro->resolution = 2000.0 / MPU6050_G;
            break;
        case MPU6050_GYRO_FREQ_1000:
            gyro->resolution = 1000.0 / MPU6050_G;
            break;
        case MPU6050_GYRO_FREQ_500:
            gyro->resolution = 500.0 / MPU6050_G;
            break;
        case MPU6050_GYRO_FREQ_250:
        default:
            gyro->resolution = 250.0 / MPU6050_G;
            break;
    }
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);
}

static void sixaxis_init_accel(struct sixaxis_device *accel, uint8_t frequency) {
    uint8_t buffer[2] = { MPU6050_ACCEL_CONFIG_REG, frequency };
    switch (frequency) {
        case MPU6050_ACCEL_FREQ_16:
            accel->resolution = 16.0 / MPU6050_G;
            break;
        case MPU6050_ACCEL_FREQ_8:
            accel->resolution = 8.0 / MPU6050_G;
            break;
        case MPU6050_ACCEL_FREQ_4:
            accel->resolution = 4.0 / MPU6050_G;
            break;
        case MPU6050_ACCEL_FREQ_2:
        default:
            accel->resolution = 2.0 / MPU6050_G;
            break;

    }
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);
}

static float pitch_angle(struct sixaxis sensor, uint16_t delta) {
    static double pitch = 0;
    double gx = (180.0 / 3.14159) * ((double) sensor.gyro.x * ((double) delta / 1000000.0));
    double ax = (180.0 / 3.14159) * ((double) sensor.accel.x * ((double) delta / 1000000.0));
    pitch = 0.98 * (pitch + gx) + 0.02 * ax;
    return pitch;
}

static float yaw_angle(struct sixaxis sensor, uint16_t delta) {
    static double yaw = 0;
    double gy = (180.0 / 3.14159) * ((double) sensor.gyro.y * ((double) delta / 1000000.0));
    double ay = (180.0 / 3.14159) * ((double) sensor.accel.y * ((double) delta / 1000000.0));
    yaw = 0.98 * (yaw + gy) + 0.02 * ay;
    return yaw;
}

uint8_t sixaxis_init(struct sixaxis *sensor) {
    uint8_t buffer[2] = { MPU6050_POWER_REG, MPU6050_POWER_CLOCK_XGYRO };

    i2c_init(i2c0, 400000); // 400 KHz clock speed

    // set up the TX pin and clock pin to pull up by default
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);   
    gpio_set_function(I2C_SCK_PIN, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCK_PIN);

    // request for the device to power itself up
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);

    if (sixaxis_test())
        return 1;

    sixaxis_init_gyro(&sensor->gyro, MPU6050_GYRO_FREQ_250);
    sixaxis_init_accel(&sensor->accel, MPU6050_ACCEL_FREQ_2);

    buffer[1] = 0; // pull down to turn on device
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);

    return 0;
}

void sixaxis_read(struct sixaxis *sensor, uint16_t delta) {    
    uint8_t buffer[14];
    uint8_t reg = MPU6050_ACCEL_VALUES_REG;

    // request a read from the accelerometer
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, &reg, 1, true);
    i2c_read_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 14, false);

    // dump the buffer data into the x, y, z fields
    bytes_to_tuple(buffer, sensor);

    sensor->pitch = pitch_angle(*sensor, delta);
    sensor->yaw = yaw_angle(*sensor, delta);
}