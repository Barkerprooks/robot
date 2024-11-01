#ifndef __sixaxis_h__
#define __sixaxis_h__

#include <stdint.h>

struct tuple {
    int16_t x, y, z;
};

struct sixaxis {
    struct tuple accel;
    struct tuple gyro;
};

void sixaxis_init();
void sixaxis_read(struct sixaxis *data);

float interpolate_angle(struct sixaxis data, float delta);

#endif