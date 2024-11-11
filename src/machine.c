#include "pico/cyw43_arch.h"

#include "machine.h"

uint8_t network_init(struct machine *robot, const char *ssid, const char *psk, uint8_t attempts) {    
    if (cyw43_arch_init())
        return ERROR_NETWORK_INIT;
    
    cyw43_arch_enable_sta_mode();

    while (attempts--) {
        if (cyw43_arch_wifi_connect_timeout_ms(ssid, psk, CYW43_AUTH_WPA2_AES_PSK, 30000) == 0)
            return NO_ERROR;
    }

    return ERROR_WIFI_CONNECT;
}

bool running(const struct machine robot) {
    return true;//robot.network == NO_ERROR && robot.sixaxis == NO_ERROR;
}