#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "machine.h"
#include "motors.h"

#include "pid.h"
#include "service.h"
#include "sixaxis.h"

#define DEBUG_PRINT_DELAY 25
#define DEBUG 1

#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

void loop(struct machine *robot, struct sixaxis *sensor) {
    while (true) {
        sixaxis_read_angle(sensor);
        // robot->speed = pid(sensor->angle, delta, 64, 4, 1);

        if (sensor->angle > 0) {
            dc_motors_move(MOTOR_DIRECTION_B, sensor->angle);
        } else if (sensor->angle < 0) {
            dc_motors_move(MOTOR_DIRECTION_F, -sensor->angle);
        }
#if DEBUG
        // printf("network: %s\n", robot->status & NETWORK_INITIALIZED_FLAG ? "connected" : "disconnected");
        // printf("sixaxis: %s\n\n", robot->status & SIXAXIS_INITIALIZED_FLAG ? "enabled" : "disabled");

        // printf(" gyro x: %f\n", (double) sensor.gyro.x * sensor.gyro.resolution);
        // printf("accel y: %f\n", (double) sensor.accel.y * sensor.accel.resolution);
        // printf("accel z: %f\n\n", (double) sensor.accel.z * sensor.accel.resolution);
        
        // printf("  angle: %f\n", sensor.angle);
        // printf("  speed: %d\n", robot->speed);

        // sleep_ms(DEBUG_PRINT_DELAY); // delay for the clear screen
        // printf("\033[2J\033[H"); // clear screen before writing, we can monitor the values easier that way
#endif
    }
}

int main() {
#if DEBUG
    stdio_init_all();
#endif
    struct service server; // network service to advertise gyro values
    struct sixaxis sensor; // stores values for the gyro sensor
    struct machine robot; // robot status and misc values not from sensors

    network_init(&robot, WIFI_SSID, WIFI_PASSWORD, 3);
    service_init(&robot, &server, 4000);
    sixaxis_init(&robot, &sensor, GYRO_FREQ_250, ACCEL_FREQ_2);

    sixaxis_set_offset(ACCEL_X_OFFSET, -349);
    sixaxis_set_offset(ACCEL_Y_OFFSET, -717);
    sixaxis_set_offset(ACCEL_Z_OFFSET, 1389);
    sixaxis_set_offset(GYRO_X_OFFSET, 66);
    sixaxis_set_offset(GYRO_Y_OFFSET, 17);
    sixaxis_set_offset(GYRO_Z_OFFSET, -20);

    dc_motors_init();

    loop(&robot, &sensor);
}
