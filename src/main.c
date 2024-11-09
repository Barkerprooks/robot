#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "lwip/udp.h"

#include <stdio.h>
#include <time.h>

#include "sixaxis.h"
#include "motors.h"
#include "pid.h"

#define DELAY 25
#define DEBUG 1

#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

#define NETWORK_INIT 0x01
#define SIXAXIS_INIT 0x10

int8_t network_init(uint8_t attempts) {    
    if (cyw43_arch_init())
        return -1;
    
    cyw43_arch_enable_sta_mode();

    while (attempts--) {
#if DEBUG
        printf("%d > attempting to connect to %s\n", attempts, WIFI_SSID);
#endif
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) == 0)
            return 0;
    }

    return -2;
}

int main() {
    stdio_init_all(); // debug

    int8_t network = network_init(5);

    uint16_t millis_send = 5000;

// NETWORKING
    struct udp_pcb *socket = udp_new();
    struct pbuf *buffer = pbuf_alloc(PBUF_TRANSPORT, 12, PBUF_RAM);
    uint8_t message[12] = "hello world\n";
    ip_addr_t ip;

    buffer->payload = message;
    buffer->len = 12;
    buffer->tot_len = 12;

    //dns_gethostbyname("hyperion.local", &ip, NULL, NULL);
    udp_sendto(socket, buffer, &ip, 4200);
// END NETWORKING

    struct sixaxis sensor; // the only sensor we have, 6axis gyro + accelerometer

    uint16_t delta = time(NULL);

    sixaxis_init(&sensor);
    motors_init();

    while (1) {
        printf("\033[2J\033[H"); // clear screen before writing, we can monitor the values easier that way

        sixaxis_read(&sensor, delta);
        delta = time(NULL) - delta; // delta should come after reading

        millis_send -= delta;
        if (millis_send < 0) {
            printf("sent message to %d\n", ip.addr);
            udp_sendto(socket, buffer, &ip, 4200);
            millis_send = 5000;
            sleep_ms(1000);
        }

        double ax = sensor.accel.x * sensor.accel.resolution;
        double ay = sensor.accel.y * sensor.accel.resolution;
        double az = sensor.accel.z * sensor.accel.resolution;
        
        double gx = sensor.gyro.x * sensor.gyro.resolution;
        double gy = sensor.gyro.y * sensor.gyro.resolution;
        double gz = sensor.gyro.z * sensor.gyro.resolution;

        printf("accel\n");
        printf("  x: %f\n", ax);
        printf("  y: %f\n", ay);
        printf("  z: %f\n", az);

        printf("gyro\n");
        printf("  x: %f\n", gx);
        printf("  y: %f\n", gy);
        printf("  z: %f\n", gz);

        printf("delta: %d\n", delta);        
        printf("pitch: %f\n", sensor.pitch);
        printf("  yaw: %f\n\n", sensor.yaw);

        printf("network: %s\n", network == 0 ? "connected" : "disconnected");

        sleep_ms(DELAY);
    }
}