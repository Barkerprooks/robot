#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "service.h"
#include "sixaxis.h"
#include "machine.h"

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

    double start, end, delta = 0.0;

    //network_init(&robot, WIFI_SSID, WIFI_PASSWORD, 5);
    //service_init(&robot, &server, 42069);
    sixaxis_init(&robot, &sensor, GYRO_FREQ_250, ACCEL_FREQ_2);

    sixaxis_set_offset(GYRO_X_OFFSET, 64); // my X gyro likes 64
    sixaxis_set_offset(ACCEL_Z_OFFSET, 1680); // this gets it close enough

    while (running(robot)) {
        start = (double) time_us_64() / 1000.0;

        sixaxis_read(&sensor);
        calculate_angle(&sensor, delta);

        end = (double) time_us_64() / 1000.0;
        delta = (end - start) * 0.001;
#if DEBUG
        printf("network: %s\n", robot.network == NO_ERROR ? "connected" : "disconnected");
        printf("sixaxis: %s\n\n", robot.sixaxis == NO_ERROR ? "enabled" : "disabled");

        printf(" gyro x: %f\n", (double) sensor.gyro.x * sensor.gyro.resolution);
        printf("accel y: %f\n", (double) sensor.accel.y * sensor.accel.resolution);
        printf("accel z: %f\n\n", (double) sensor.accel.z * sensor.accel.resolution);
        
        printf("  angle: %f\n", sensor.angle);
        printf("  speed: %d\n", robot.speed);

        sleep_ms(DEBUG_PRINT_DELAY); // delay for the clear screen
        printf("\033[2J\033[H"); // clear screen before writing, we can monitor the values easier that way
#endif
    }

}