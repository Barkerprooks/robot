#ifndef __dhcp_h__
#define __dhcp_h__

#include "lwip/ip_addr.h"

struct dhcp_server_lease {
    uint8_t address[6];
    uint16_t expires; // timestamp when mac is no longer valid
};

struct dhcp_server {
    struct dhcp_server_lease leases[2]; // only allow 2 devices to connect
    struct udp_pcb *udp;
    ip_addr_t ip, nm;
};

void init_dhcp_server(struct dhcp_server *server, ip_addr_t *ip, ip_addr_t *nm);
void stop_dhcp_server(struct dhcp_server *server);

#endif