#ifndef __sixaxis_h__
#define __sixaxis_h__

#include <stdbool.h>
#include <stdint.h>

#define SPI_SDA_PIN 4 // a.k.a the TX pin
#define SPI_SCK_PIN 5 // a.k.a the clock pin

#define MPU6050_DEVICE_ID 0x68
#define MPU6050_ACCEL_REG 0x3b
#define MPU6050_GYRO_REG 0x43

struct tuple {
    int16_t x, y, z;
};

void bytes_to_tuple(uint8_t buffer[6], struct tuple *data);

struct sixaxis {
    struct tuple accel;
    struct tuple gyro;
};

void init_sixaxis();
bool read_sixaxis(struct sixaxis *data);

void reset_sixaxis();

#endif