#include "pico/cyw43_arch.h"

#include "machine.h"

void network_init(struct machine *robot, const char *ssid, const char *psk, uint8_t attempts) {    
    robot->status ^= NETWORK_INITIALIZED_FLAG; // assume we've failed
    
    if (strlen(ssid) == 0 || strlen(psk) == 0 || cyw43_arch_init())
        return; // yup
    
    cyw43_arch_enable_sta_mode();

    while (attempts--) {
        if (cyw43_arch_wifi_connect_timeout_ms(ssid, psk, CYW43_AUTH_WPA2_AES_PSK, 30000) == 0) {
            robot->status |= NETWORK_INITIALIZED_FLAG;
            break; // if we connected successfully break the loop and end the function
        }
    }

}