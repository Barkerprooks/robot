#include "pico/stdlib.h"

#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "sixaxis.h"
#include "motors.h"

#define KP 25.0 // error gain
#define KI 0.0 // integral gain
#define KD 0.8 // derivative gain

#define ACCEL_SCALE 16384.0
#define GYRO_SCALE 131.0

void init_robot(bool debug) {
    if (debug)
        stdio_init_all();

    printf("initializing sixaxis sensor\n");
    init_sixaxis();

    printf("initializing motors\n");
    init_motors();
}

// composes the acceleration angle
float angle(struct sixaxis data, float delta) {
    float denominator = sqrt(pow(data.accel.x / ACCEL_SCALE, 2) + pow(data.accel.z / ACCEL_SCALE, 2));
    float accel_angle = atan((data.accel.y / ACCEL_SCALE) / denominator) * 57.29577950560105;
    float gyro_angle = data.gyro.x / 131.0;
    return 0.98 * (gyro_angle * delta) + 0.02 * accel_angle;
}

// pid algorithm tending towards zero (0.0)
float pid_zero(struct sixaxis data, float delta) {
    static float prev = 0, i = 0;
    float p, d;

    p = KP * (angle(data, delta) - 0); // calculate error
    // i += KI * p; // update integral (is there no reason to do this???)
    d = KD * ((p - prev) / delta); // calculate derivative
    prev = p;
    
    return p + d;
}

int main() {
    struct sixaxis data;

    printf("initializing robot\n");
    init_robot(true);

    float pid, delta = (float) time(NULL);

    while (true) {
        delta = (time(NULL) - delta) / 1000;

        read_sixaxis(&data);
        
        printf("accel -> x: %06lu, y: %06lu, z: %06lu\n", data.accel.x, data.accel.y, data.accel.z);
        printf("gyro  -> x: %06lu, y: %06lu, z: %06lu\n", data.gyro.x, data.gyro.y, data.gyro.z);
    
        //pid = pid_zero(data, delta);

        //printf("PID: %0.4f\n", pid);

        sleep_ms(10);
    }

    return 0;
}