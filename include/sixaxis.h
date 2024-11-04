#ifndef __sixaxis_h__
#define __sixaxis_h__

#include <stdint.h>

struct sixaxis_device {
    double resolution;
    int16_t x, y, z;
};

struct sixaxis {
    struct sixaxis_device accel;
    struct sixaxis_device gyro;
    double pitch, yaw;
};

uint8_t sixaxis_init(struct sixaxis *sensor);
void sixaxis_read(struct sixaxis *sensor, uint16_t delta);

#endif