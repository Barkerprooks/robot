#include "pico/stdlib.h"

#include <stdbool.h>
#include <stdio.h>

#include "accelerometer.h"
#include "motors.h"

bool initialize_robot(bool debug) {
    if (debug)
        stdio_init_all();

    initialize_motors();

    return initialize_accelerometer(1600, 2);
}

int main() {

    if (initialize_robot(true)) {
        while (true) {
            int16_t x = accelerometer_get_x();
            int16_t y = accelerometer_get_y();
            int16_t z = accelerometer_get_z();

            printf("x: %ld, y: %ld, z: %ld\n", x, y, z);
        }
    }

    return 0;
}