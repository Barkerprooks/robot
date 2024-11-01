#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include <stdio.h>

#include "sixaxis.h"
#include "motors.h"
#include "dhcp.h"
#include "dns.h"


int main() {
    // networking 
    ip4_addr_t gateway, netmask; // we are the gateway
    struct dhcp_server dhcpd; // allows the pi to assign IP addresses
    struct dns_server dnsd; // allows host discovery

    // meatspace
    struct sixaxis sensor; // the only sensor we have, 6axis gyro + accelerometer

    cyw43_arch_init(); // enable networking kernel
    stdio_init_all();  // allow STDOUT over USB

    // start up WiFi AP, idc if theres no password
    cyw43_arch_enable_ap_mode("robotuah", NULL, CYW43_AUTH_OPEN);

    // initialize basic networking services
    IP4_ADDR(&gateway, 192, 168, 0, 1); // _gateway on the first address

    dhcp_server_init(&dhcpd, gateway);

    // initialize peripherials
    sixaxis_init();
    motors_init();

    while (1) {
        sixaxis_read(&sensor);

        printf("gyro  -> x: %06d, y: %06d, z: %06d\n", 
            sensor.gyro.x, 
            sensor.gyro.y, 
            sensor.gyro.z);
        
        sleep_ms(100);
    }

    return 0;
}