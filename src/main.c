#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "cyw43_config.h"

#include <stdio.h>

#include "sixaxis.h"
#include "motors.h"
#include "dhcp.h"
#include "dns.h"

int main() {
    // below: networking (AP version, the robot can host its own network)
    // still under construction
#ifdef AP_MODE
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
    // just connect to local network for now
    if (cyw43_arch_init()) {
        printf("no networking :(\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    
    if (cyw43_arch_wifi_connect_timeout_ms("ssid", "password", CYW43_AUTH_WPA3_WPA2_AES_PSK, 30000)) {
        printf("failed to connect\n");
    } else {
        printf("connected!\n");
    }
#endif

    // meatspace
    struct sixaxis sensor; // the only sensor we have, 6axis gyro + accelerometer

    // initialize peripherials
    sixaxis_init();
    motors_init();

    while (1) {
        sixaxis_read(&sensor);

        printf(cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP ? "connected\n" : "disconnected\n");
        printf("gyro  -> x: %06d, y: %06d, z: %06d\n", 
            sensor.gyro.x, 
            sensor.gyro.y, 
            sensor.gyro.z);
        
        sleep_ms(100);
    }

    return 0;
}