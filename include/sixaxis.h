#ifndef __sixaxis_h__
#define __sixaxis_h__

#include <stdbool.h>
#include <stdint.h>

struct tuple {
    int16_t x, y, z;
};

struct sixaxis {
    struct tuple accel;
    struct tuple gyro;
};

void init_sixaxis();
bool read_sixaxis(struct sixaxis *data);

void reset_sixaxis();

#endif