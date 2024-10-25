#ifndef __accelerometer_h__
#define __accelerometer_h__

#define REG_ACCELEROMETER_DEVICE_ID 0x0f
#define REG_ACCELEROMETER_CONTROL_1 0x20
#define REG_ACCELEROMETER_CONTROL_4 0x23
#define REG_ACCELEROMETER_X 0x28
#define REG_ACCELEROMETER_Y 0x2a
#define REG_ACCELEROMETER_Z 0x2c

#include <stdbool.h>
#include <stdint.h>

// implementation for LIS3DH on an SPI interface

bool initialize_accelerometer(const uint16_t sample_rate, const uint8_t range);
int16_t accelerometer_get_x();
int16_t accelerometer_get_y();
int16_t accelerometer_get_z();

#endif