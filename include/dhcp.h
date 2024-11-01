#ifndef __dhcp_h__
#define __dhcp_h__

#include "lwip/ip_addr.h"

#define DHCP_MAX_LEASES 14 // hardcoded /28 netmask

struct dhcp_server_lease {
    uint8_t address[6];
    uint16_t expires; // timestamp when mac is no longer valid
};

struct dhcp_server {
    struct dhcp_server_lease leases[8]; // only allow 8 devices to connect
    ip_addr_t gateway, netmask; // netmask will be set to 255.255.255.240
    struct udp_pcb *socket; // lwip UDP socket
};

void dhcp_server_init(struct dhcp_server *server, const ip_addr_t gateway);
// TODO: implement a function to free the UDP pointer, we dont need right now bc the server never closes

#endif