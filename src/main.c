#include <stdio.h>
#include "pico/stdlib.h"

#include "service.h"
#include "sixaxis.h"
#include "machine.h"
#include "pid.h"

#define DEBUG_PRINT_DELAY 25
#define DEBUG 1

#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

int main() {
#if DEBUG
    stdio_init_all();
#endif

    struct service server; // network service to advertise gyro values
    struct sixaxis sensor; // stores values for the gyro sensor
    struct machine robot; // robot status and misc values not from sensors

    double start, stop, delta, pid;

    //network_init(&robot, WIFI_SSID, WIFI_PASSWORD, 5);
    //service_init(&robot, &server, 42069);
    sixaxis_init(&robot, &sensor, GYRO_FREQ_250, ACCEL_FREQ_2);

    // no offset is good enough for now
    sixaxis_set_offset(ACCEL_Y_OFFSET, 1593);
    sixaxis_set_offset(ACCEL_Z_OFFSET, 1000);
    sixaxis_set_offset(GYRO_X_OFFSET, 58);
    
    while (running(robot)) {
        start = (double) time_us_64() / 1000.0;
        
        sixaxis_read(&sensor, delta);

        stop = (double) time_us_64() / 1000.0;
        delta = (stop - start) * 0.001;
#if DEBUG
        printf("network: %s\n", robot.network == NO_ERROR ? "connected" : "disconnected");
        printf("sixaxis: %s\n\n", robot.sixaxis == NO_ERROR ? "enabled" : "disabled");

        printf(" gyro x: %f\n", (double) sensor.gyro.x * sensor.gyro.resolution);
        printf("accel y: %f\n", (double) sensor.accel.y * sensor.accel.resolution);
        printf("accel z: %f\n", (double) sensor.accel.z * sensor.accel.resolution);
        printf("angle: %f\n\n", sensor.angle);

        printf("speed: %d\n", robot.speed);
        printf("delta: %f\n", delta);

        sleep_ms(DEBUG_PRINT_DELAY); // delay for the clear screen
        printf("\033[2J\033[H"); // clear screen before writing, we can monitor the values easier that way
#endif
    }

}