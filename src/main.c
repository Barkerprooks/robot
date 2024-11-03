#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include <stdio.h>

#include "sixaxis.h"
#include "motors.h"
#include "pid.h"

#define AP_MODE 0
#define DELAY 10

#if AP_MODE
#include "dhcp.h"
#include "dns.h"
#endif 

int main() {

    stdio_init_all();

    // below: networking (AP version, the robot can host its own network)
    // still under construction
#if AP_MODE
    ip4_addr_t gateway, netmask; // we are the gateway
    struct dhcp_server dhcpd; // allows the pi to assign IP addresses
    struct dns_server dnsd; // allows host discovery

    cyw43_arch_init(); // enable networking kernel
    stdio_init_all();  // allow STDOUT over USB

    // start up WiFi AP, idc if theres no password
    cyw43_arch_enable_ap_mode("robotuah", NULL, CYW43_AUTH_OPEN);

    // initialize basic networking services
    IP4_ADDR(&gateway, 192, 168, 0, 1); // _gateway on the first address

    dhcp_server_init(&dhcpd, gateway);
#else
    // if (cyw43_arch_init()) {
    //     printf("no networking :(\n");
    //     return 1;
    // }

    // cyw43_arch_enable_sta_mode();
    // cyw43_arch_wifi_connect_timeout_ms("Frontier0016", "LemonHorse3419", CYW43_AUTH_WPA2_AES_PSK, 30000);
#endif

    //printf("%d\n", cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA));

    // meatspace
    struct sixaxis sensor; // the only sensor we have, 6axis gyro + accelerometer
    float angle, power;
    uint64_t count = 0;

    gpio_init(14);
    gpio_init(15);

    gpio_set_dir(14, GPIO_OUT);
    gpio_set_dir(15, GPIO_OUT);

    // initialize peripherials
    sixaxis_init();
    // motors_init();

    while (1) {
        printf("\033[2J\033[H"); // clear screen before writing, we can monitor the values easier that way

        sixaxis_read(&sensor);

        angle = interpolate_angle(sensor, 1);
        power = pid(angle, 1); // correcting value to get angle to zero

        //printf(cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP ? "connected\n" : "disconnected\n");
        printf("accel -> x: %05d, y: %05d, z: %05d\n", 
            sensor.accel.x,
            sensor.accel.y, 
            sensor.accel.z);
        printf("gyro  -> x: %05d, y: %05d, z: %05d\n", 
            sensor.gyro.x, 
            sensor.gyro.y, 
            sensor.gyro.z);
        
        printf("angle: %.1f degrees\n", angle);
        printf("power: %.1f\n", power);
        printf("count: %09llu\n", count++);

        if (power > 0) {
            gpio_put(14, true);
            gpio_put(15, false);
        } else {
            gpio_put(14, false);
            gpio_put(15, true);
        }

        sleep_ms(DELAY);
    }

    return 0;
}