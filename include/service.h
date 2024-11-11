#ifndef __service_h__
#define __service_h__

#include "lwip/udp.h"
#include "machine.h"

struct service {
    struct udp_pcb *socket;
};

// service that listens on the specified port and serves UDP packets to the connecting client
uint8_t service_init(struct machine *robot, struct service *server, const uint16_t port);

#endif