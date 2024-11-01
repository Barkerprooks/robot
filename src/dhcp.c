#include "lwip/ip4_addr.h"
#include "lwip/udp.h"
#include "cyw43_config.h"

#include <string.h>

#include "dhcp.h"

#define SERVER_PORT 67
#define CLIENT_PORT 68

#define LEASE_TIME 86400

#define OPCODE_DISCOVER   0x01
#define OPCODE_OFFER      0x02
#define OPCODE_REQUEST    0x03
#define OPCODE_DECLINE    0x04
#define OPCODE_ACK        0x05
#define OPCODE_NACK       0x06
#define OPCODE_RELEASE    0x07
#define OPCODE_INFORM     0x08

#define OPTION_PAD                0x00
#define OPTION_SUBNET_MASK        0x01
#define OPTION_ROUTER             0x03
#define OPTION_DNS                0x06
#define OPTION_HOST_NAME          0x0c
#define OPTION_REQUESTED_IP       0x32
#define OPTION_IP_LEASE_TIME      0x33
#define OPTION_MSG_TYPE           0x35
#define OPTION_SERVER_ID          0x36
#define OPTION_PARAM_REQUEST_LIST 0x37
#define OPTION_MAX_MSG_SIZE       0x39
#define OPTION_VENDOR_CLASS_ID    0x3c
#define OPTION_CLIENT_ID          0x3d
#define OPTION_END                0xff

#define DHCP_BOOT_FILE_LENGTH 128
#define DHCP_OPTIONS_LENGTH 312
#define DHCP_SERVER_HOSTNAME_LENGTH 64

#define EMPTY_MAC_ADDRESS "\x00\x00\x00\x00\x00\x00"

struct dhcp_message {
    uint8_t opcode, hwtype, hwsize, whops;
    uint32_t id;
    uint16_t elapsed, flags;

    uint8_t client_address[4];
    uint8_t your_address[4];
    uint8_t server_address[4];
    uint8_t gateway_address[4];

    uint8_t client_mac[16];

    uint8_t server_hostname[DHCP_SERVER_HOSTNAME_LENGTH];
    uint8_t boot_file[DHCP_BOOT_FILE_LENGTH];
    uint8_t options[DHCP_OPTIONS_LENGTH];
};

static uint8_t *parse_option(uint8_t *options, const uint8_t option) {
    // options should be shifted over by 4 bytes now
    // to find the next offset, get the value at the next index, and add 2 to it
    // keep going until i is out of bounds or we hit an `option end` byte
    for (uint8_t i = 0; i < DHCP_OPTIONS_LENGTH - 4 && options[i] != OPTION_END; i += 2 + options[i + 1])
        return &options[i];
    return NULL;
}

static void write_option(uint8_t **options, const uint8_t option, const void *value, const size_t n) {
    uint8_t *options_buffer = *options;
    *options_buffer++ = option; // sets option to first byte, shifts ref to next byte
    *options_buffer++ = n; // sets size of data, shifts ref to next byte
    
    memcpy(options_buffer, value, n); // copies data into the buffer
    *options = options_buffer + n; // shift buffer based on size of data
}

static bool dhcp_process_discover(uint8_t **options, struct dhcp_server *server, struct dhcp_message *message) {
    uint8_t lease_index = DHCP_MAX_LEASES;
    uint8_t opcode[1] = { OPCODE_OFFER };
    
    // look for an empty lease or match the MAC up with its previous lease
    for (uint8_t i = 0; i < DHCP_MAX_LEASES; ++i) {
        
        if (memcmp(server->leases[i].address, message->client_mac, 6) == 0) {
            lease_index = i;
            break;
        }
        
        if (lease_index == DHCP_MAX_LEASES) {
            uint32_t expires = server->leases[i].expires << 16 | 0xffff;

            if ((int32_t)(expires - cyw43_hal_ticks_ms()) < 0)
                memset(server->leases[i].address, 0, 6); // if the address there has expired, zero it out

            if (memcmp(server->leases[i].address, EMPTY_MAC_ADDRESS, 6) == 0)
                lease_index = i;
        }
    }

    // if we didn't find any available address spaces, dont do anything
    if (lease_index == DHCP_MAX_LEASES) {
        return false;
    }

    message->your_address[3] = lease_index; // asign the ip
    write_option(options, OPTION_MSG_TYPE, opcode, 1);

    return true;
}

static bool dhcp_process_request(uint8_t **options, struct dhcp_server *server, struct dhcp_message *message) {
    uint8_t *option = parse_option(*options, OPTION_REQUESTED_IP);
    uint8_t lease_index = option[5];

    // if (memcmp(server->leases[].address, ))

    return false;
}

static void dhcp_process(void *arg, struct udp_pcb *socket, struct pbuf *pbuf, const ip_addr_t *src, uint16_t port) {
    struct dhcp_server *server = arg;
    struct dhcp_message message;
    uint8_t *options, *message_type;

    if (pbuf->tot_len < 243) {
        pbuf_free(pbuf);
        return;
    }

    pbuf_copy_partial(pbuf, &message, sizeof(message), 0); // grab the message from the client

    message.opcode = OPCODE_OFFER;

    // the line below copies 32 bits into an 8x4 byte array space
    memcpy(&message.your_address, &ip4_addr_get_u32(ip_2_ip4(&server->gateway)), 4);

    options = (uint8_t *) &message.options; // grab a reference to the options buffer
    options += 4; // offset the buffer by 4. the first 4 bytes SHOULD be 99. 130, 83, 99, but we'll ignore them

    message_type = parse_option(options, OPTION_MSG_TYPE);

    switch(message_type[2]) {
        case OPCODE_DISCOVER:
            if (!dhcp_process_discover(&options, server, &message)) {
                pbuf_free(pbuf);
                return;
            }
            break;
        case OPCODE_REQUEST:
            if (!dhcp_process_request(&options, server, &message)) {
                pbuf_free(pbuf);
                return;
            }
            break;
        default: 
            pbuf_free(pbuf);
            return;
    }

    pbuf_free(pbuf);
}

void dhcp_server_init(struct dhcp_server *server, const ip_addr_t gateway) {
    ip_addr_t netmask;

    IP4_ADDR(&netmask, 255, 255, 255, 240); // we're on a pico, this might even be too much
    
    // set up the server
    ip_addr_copy(server->gateway, gateway);
    ip_addr_copy(server->netmask, netmask);
    memset(server->leases, 0, sizeof(server->leases));

    // NOTE: allocating memory, make sure to free if this process will end before the program does 
    server->socket = udp_new();

    udp_recv(server->socket, dhcp_process, (void *) server);
    udp_bind(server->socket, IP_ANY_TYPE, SERVER_PORT);
}