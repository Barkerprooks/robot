#include "accelerometer.h"

#include "pico/stdlib.h"

#include <stdbool.h>
#include <stdio.h>

int main() {

    stdio_init_all();
    
    if (initialize_accelerometer(1600, 2)) {
        while (true) {
            int16_t x = accelerometer_get_x();
            int16_t y = accelerometer_get_y();
            int16_t z = accelerometer_get_z();

            printf("x: %ld, y: %ld, z: %ld\n", x, y, z);
        }
    }

    return 0;
}