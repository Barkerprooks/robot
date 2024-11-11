#include "service.h"

uint8_t service_init(struct machine *robot, struct service *server, const uint16_t port) {
    server->socket = udp_new();
    udp_bind(server->socket, IP4_ADDR_ANY4, port);
}