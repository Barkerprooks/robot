#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include <stdio.h>

#include <stdlib.h>
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

#define G 32768.0


static bool sixaxis_failed_to_respond() {
    uint8_t buffer = MPU6050_WHO_AM_I_REG;

    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, &buffer, 1, true);
    i2c_read_blocking(i2c0, MPU6050_DEVICE_ID, &buffer, 1, false);

    return buffer == MPU6050_DEVICE_ID ? false : true;
}

static void sixaxis_init_gyro(struct sixaxis_dof *gyro, const uint8_t frequency) {
    uint8_t buffer[2] = { MPU6050_GYRO_CONFIG_REG, frequency };
    switch (frequency) {
        case GYRO_FREQ_2000:
            gyro->resolution = 2000.0 / G;
            break;
        case GYRO_FREQ_1000:
            gyro->resolution = 1000.0 / G;
            break;
        case GYRO_FREQ_500:
            gyro->resolution = 500.0 / G;
            break;
        case GYRO_FREQ_250:
        default:
            gyro->resolution = 250.0 / G;
            break;
    }
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);
}

static void sixaxis_init_accel(struct sixaxis_dof *accel, const uint8_t frequency) {
    uint8_t buffer[2] = { MPU6050_ACCEL_CONFIG_REG, frequency };
    switch (frequency) {
        case ACCEL_FREQ_16:
            accel->resolution = 16.0 / G;
            break;
        case ACCEL_FREQ_8:
            accel->resolution = 8.0 / G;
            break;
        case ACCEL_FREQ_4:
            accel->resolution = 4.0 / G;
            break;
        case ACCEL_FREQ_2:
        default:
            accel->resolution = 2.0 / G;
            break;
    }
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);
}

static void sixaxis_read_values(struct sixaxis *sensor) {
    // start reading from accelerometer x register offset.
    // they are all next to each other
    // the gyroscope starts at 1 byte away from the accelerometer
    // so its faster to just read 14 bytes raw and pull out the needed
    // values
    uint8_t reg = MPU6050_ACCEL_VALUES_REG;
    uint8_t buffer[14];

    // request a read from the accelerometer
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, &reg, 1, true);
    i2c_read_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 14, false);

    // dump the buffer data into the x, y, z fields
    // takes a 14 byte raw read from the i2c bus and decodes the values
    // into the proper devices
    // [ | | | | | | | | | | | | | ] <- buffer layout
    //   X   Y   Z       X   Y   Z   <- accel / gyro axis (2-bytes each, LE)
    
    // accelerometer
    sensor->accel.x = (buffer[0] << 8 | buffer[1]);
    sensor->accel.y = (buffer[2] << 8 | buffer[3]);
    sensor->accel.z = (buffer[4] << 8 | buffer[5]);
    
    // gyroscope
    sensor->gyro.x = (buffer[8] << 8 | buffer[9]);
    sensor->gyro.y = (buffer[10] << 8 | buffer[11]);
    sensor->gyro.z = (buffer[12] << 8 | buffer[13]);
}

static void sixaxis_averages(struct sixaxis *sensor, int16_t avgs[6]) {
    int64_t sums[6] = {0, 0, 0, 0, 0, 0};
    uint16_t i;

    for (i = 0; i < 512; i++) {
        sixaxis_read_values(sensor);
        if (i > 100) { // first 100 reads are more likely to be outliers
            sums[0] += sensor->accel.x;
            sums[1] += sensor->accel.y;
            sums[2] += sensor->accel.z;
            sums[3] += sensor->gyro.x;
            sums[4] += sensor->gyro.y;
            sums[5] += sensor->gyro.z;
        }
        sleep_ms(2); // disallows duplicate values
    }

    for (i = 0; i < 6; i++)
        avgs[i] = sums[i] / 412;
}

static void sixaxis_calibrate(struct sixaxis *sensor) {
    // this is basically the arduino IMU calibration code, but in pico-sdk
    uint8_t i, regs[6] = { ACCEL_X_OFFSET, ACCEL_Y_OFFSET, ACCEL_Z_OFFSET, GYRO_X_OFFSET, GYRO_Y_OFFSET, GYRO_Z_OFFSET };
    int16_t avgs[6], offsets[6], targets[6] = {1, 1, 16384, 2, 2, 2};

    // set all offsets to 0
    for (i = 0; i < 6; i++)
        sixaxis_set_offset(regs[i], 0);

    sixaxis_averages(sensor, avgs);

    offsets[0] = -avgs[0] / 8;
    offsets[1] = -avgs[1] / 8;
    offsets[2] = (16384 - avgs[2]) / 8;
    offsets[3] = -avgs[3] / 8;
    offsets[4] = -avgs[4] / 8;
    offsets[5] = -avgs[5] / 8;

    for (i = 0; i < 6; i++) {

        while (abs(targets[i] - abs(avgs[i])) >= 2) { // calibrate universally to a deadzone of 4
            switch (i) {
                case 0:
                case 1:
                    offsets[i] = (offsets[i] - (avgs[i] / 8));
                    sixaxis_set_offset(regs[i], offsets[i]);
                    break;
                case 2:
                    // z accel is special because gravity
                    offsets[i] = offsets[i] + ((16384 - avgs[i]) / 8);
                    sixaxis_set_offset(regs[i], offsets[i]);
                    break;
                case 3:
                case 4:
                case 5:
                    offsets[i] = (offsets[i] - (avgs[i] / 6));
                    sixaxis_set_offset(regs[i], offsets[i]);
                    break;
            }

            sixaxis_averages(sensor, avgs);

            printf("ACCEL X => mean: %d, offset: %d\n", avgs[0], offsets[0]);
            printf("ACCEL Y => mean: %d, offset: %d\n", avgs[1], offsets[1]);
            printf("ACCEL Z => mean: %d, offset: %d\n", avgs[2], offsets[2]);
            printf("GYRO X => mean: %d, offset: %d\n", avgs[3], offsets[3]);
            printf("GYRO Y => mean: %d, offset: %d\n", avgs[4], offsets[4]);
            printf("GYRO Z => mean: %d, offset: %d\n", avgs[5], offsets[5]);
            printf("calibrating...\n");
        }
    }
}

// PUBLIC MEMBERS
void sixaxis_set_offset(const uint8_t device_axis_reg, const uint16_t offset) {
    uint8_t buffer[3] = { device_axis_reg, (offset & 0xff00) >> 8, offset & 0xff };

    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 3, false);
}

void sixaxis_init(struct machine *robot, struct sixaxis *sensor, const uint8_t gyro_freq, const uint8_t accel_freq) {
    uint8_t buffer[2] = { MPU6050_POWER_REG, MPU6050_POWER_CLOCK_XGYRO }; // initialize clock on x axis

    i2c_init(i2c0, 400000); // 400 KHz clock speed

    // set up the TX pin and clock pin to pull up by default
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);   
    gpio_set_function(I2C_SCK_PIN, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCK_PIN);

    // request for the device to power itself up
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);

    // set up the device if it's there
    if (sixaxis_failed_to_respond()) {
        robot->status ^= SIXAXIS_INITIALIZED_FLAG; // remove flag if its there
    } else {
        sixaxis_init_gyro(&sensor->gyro, gyro_freq);
        sixaxis_init_accel(&sensor->accel, accel_freq);
    
        buffer[1] = 0; // keep power register in the first index, set pull down to turn on device
        i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);

        // sixaxis_calibrate(sensor);

        robot->status |= SIXAXIS_INITIALIZED_FLAG;
    }
}

void sixaxis_read_angle(struct sixaxis *sensor, const double delta) {
    static double delta, previous_angle = 0.0;
    double accel_angle, gyro_angle;

    sixaxis_read_values(sensor);

    // calculate the tilt angle (left to right)
    accel_angle = atan2((double) sensor->accel.y, (double) sensor->accel.z) * 57.2958; // atan2() * (180 / pi) = degrees
    gyro_angle = sensor->gyro.x * delta;

    sensor->angle = 0.9934 * (previous_angle + gyro_angle) + 0.0066 * accel_angle;
    
    previous_angle = sensor->angle;
}