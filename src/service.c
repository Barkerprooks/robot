#include "service.h"
#include <string.h>


void recv_fn(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port) {
    struct machine *robot = (struct machine *) arg;
    struct pbuf *send = pbuf_alloc(PBUF_TRANSPORT, 128, PBUF_RAM);
    char message[128];

    if (strncmp(p->payload, "getspeed", 8) == 0) {
        snprintf(message, 128, "speed: %d\n", robot->speed);
        send->payload = message;
        send->len = 128;
        send->tot_len = 128;
        udp_sendto(pcb, send, addr, port);
    }


}

void service_init(struct machine *robot, struct service *server, const uint16_t port) {
    server->socket = udp_new();

    udp_bind(server->socket, IP4_ADDR_ANY, port);
    udp_recv(server->socket, recv_fn, (void *) robot);
}