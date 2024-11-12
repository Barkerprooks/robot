#include "hardware/gpio.h"
#include "hardware/i2c.h"

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

#define MPU6050_G 32768.0

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
            gyro->resolution = 2000.0 / MPU6050_G;
            break;
        case GYRO_FREQ_1000:
            gyro->resolution = 1000.0 / MPU6050_G;
            break;
        case GYRO_FREQ_500:
            gyro->resolution = 500.0 / MPU6050_G;
            break;
        case GYRO_FREQ_250:
        default:
            gyro->resolution = 250.0 / MPU6050_G;
            break;
    }
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);
}

static void sixaxis_init_accel(struct sixaxis_dof *accel, const uint8_t frequency) {
    uint8_t buffer[2] = { MPU6050_ACCEL_CONFIG_REG, frequency };
    switch (frequency) {
        case ACCEL_FREQ_16:
            accel->resolution = 16.0 / MPU6050_G;
            break;
        case ACCEL_FREQ_8:
            accel->resolution = 8.0 / MPU6050_G;
            break;
        case ACCEL_FREQ_4:
            accel->resolution = 4.0 / MPU6050_G;
            break;
        case ACCEL_FREQ_2:
        default:
            accel->resolution = 2.0 / MPU6050_G;
            break;
    }
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);
}

static void calculate_angle(struct sixaxis *sensor, const double delta) {
    static double previous_angle = 0.0;
    
    double accel_angle = atan2((double) sensor->accel.y, (double) sensor->accel.z) * 57.2958; // atan2() * (180 / pi) = degrees
    double gyro_angle = sensor->gyro.x * delta;

    sensor->angle = 0.9934 * (previous_angle + gyro_angle) + 0.0066 * accel_angle;
    
    previous_angle = sensor->angle;
}

void sixaxis_set_offset(uint8_t device_axis_reg, uint16_t offset) {
    uint8_t buffer[3] = { device_axis_reg, (offset & 0xff) >> 8, offset & 0xff };
    i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 3, false);
}

void sixaxis_calibrate(struct sixaxis *sensor) {
    
    // set all offsets to 0
    sixaxis_set_offset(ACCEL_X_OFFSET, 0);
    sixaxis_set_offset(ACCEL_Y_OFFSET, 0);
    sixaxis_set_offset(ACCEL_Z_OFFSET, 0);
    sixaxis_set_offset(GYRO_X_OFFSET, 0);
    sixaxis_set_offset(GYRO_Y_OFFSET, 0);
    sixaxis_set_offset(GYRO_Z_OFFSET, 0);

    sleep_ms(500);
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
        robot->sixaxis = SIXAXIS_INIT_ERROR;
    } else {
        sixaxis_init_gyro(&sensor->gyro, gyro_freq);
        sixaxis_init_accel(&sensor->accel, accel_freq);
    
        buffer[1] = 0; // keep power register in the first index, set pull down to turn on device
        i2c_write_blocking(i2c0, MPU6050_DEVICE_ID, buffer, 2, false);
        robot->sixaxis = NO_ERROR;
    }
}

void sixaxis_read(struct sixaxis *sensor, const double delta) {
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

    // calculate the angle using the values we just read
    calculate_angle(sensor, delta);
}