#ifndef __system_h__
#define __system_h__

#include <stdbool.h>
#include <stdint.h>

#define NO_ERROR 0

#define NETWORK_INITIALIZED_FLAG 0b00000001
#define SIXAXIS_INITIALIZED_FLAG 0b00000010
#define SERVICE_INITIALIZED_FLAG 0b00000100

// status = 8 flags = 8 bits in the byte
// 0 = network initialized
// 1 = sixaxis initialized
// 2 = service initialized
// ... the rest will come later 

struct machine {
    uint8_t status;
    char name[128];
    int8_t speed;
};

void network_init(struct machine *robot, const char *ssid, const char *psk, uint8_t attempts);

#endif