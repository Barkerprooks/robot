#ifndef __service_h__
#define __service_h__

#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "machine.h"

struct service {
    struct tcp_pcb *cmd; // TCP socket allows for remote commands
    struct udp_pcb *out; // UDP socket provides a data stream that decodes to robot status
};

// service that listens on the specified port and serves UDP packets to the connecting client
void service_init(struct machine *robot, struct service *server, const uint16_t cmd_port, const uint16_t out_port);

#endif