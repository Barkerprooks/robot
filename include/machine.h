#ifndef __system_h__
#define __system_h__

#include <stdbool.h>
#include <stdint.h>

#define NO_ERROR 0

#define ERROR_NETWORK_INIT 1
#define ERROR_WIFI_CONNECT 2

struct machine {
    uint8_t network;
    uint8_t sixaxis;
    int8_t speed;
};

uint8_t network_init(struct machine *robot, const char *ssid, const char *psk, uint8_t attempts);
bool running(const struct machine robot);

#endif