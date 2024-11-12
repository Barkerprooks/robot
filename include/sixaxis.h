#ifndef __sixaxis_h__
#define __sixaxis_h__

#include "machine.h"

#define SIXAXIS_INIT_ERROR 1
#define SIXAXIS_READ_ERROR 2

// gyro settings
#define GYRO_FREQ_250  0
#define GYRO_FREQ_500  1
#define GYRO_FREQ_1000 2
#define GYRO_FREQ_2000 3

// accelerometer settings
#define ACCEL_FREQ_2   0
#define ACCEL_FREQ_4   1
#define ACCEL_FREQ_8   2
#define ACCEL_FREQ_16  3

// option for setting offset
#define ACCEL_X_OFFSET 0x06
#define ACCEL_Y_OFFSET 0x08
#define ACCEL_Z_OFFSET 0x0a
#define GYRO_X_OFFSET  0x13
#define GYRO_Y_OFFSET  0x15
#define GYRO_Z_OFFSET  0x17

struct sixaxis_dof {
    double resolution;
    int16_t x, y, z;
};

struct sixaxis {
    struct sixaxis_dof accel;
    struct sixaxis_dof gyro;
    double angle;
};

void calculate_angle(struct sixaxis *sensor, const double delta);
void sixaxis_set_offset(const uint8_t device_axis_reg, const uint16_t offset);
uint16_t sixaxis_get_offset(const uint8_t device_axis_reg);

void sixaxis_init(struct machine *robot, struct sixaxis *sensor, const uint8_t gyro_freq, const uint8_t accel_freq);
void sixaxis_read(struct sixaxis *sensor);

#endif