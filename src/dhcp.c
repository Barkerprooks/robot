#include "dhcp.h"

#define SERVER_PORT 67
#define CLIENT_PORT 68

#define LEASE_TIME 86400

#define DISCOVER   0x01
#define OFFER      0x02
#define REQUEST    0x03
#define DECLINE    0x04
#define ACK        0x05
#define NACK       0x06
#define RELEASE    0x07
#define INFORM     0x08

#define OPT_PAD                0x00
#define OPT_SUBNET_MASK        0x01
#define OPT_ROUTER             0x03
#define OPT_DNS                0x06
#define OPT_HOST_NAME          0x0c
#define OPT_REQUESTED_IP       0x32
#define OPT_IP_LEASE_TIME      0x33
#define OPT_MSG_TYPE           0x35
#define OPT_SERVER_ID          0x36
#define OPT_PARAM_REQUEST_LIST 0x37
#define OPT_MAX_MSG_SIZE       0x39
#define OPT_VENDOR_CLASS_ID    0x3c
#define OPT_CLIENT_ID          0x3d
#define OPT_END                0xff

#define IPV4(a, b, c, d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))

struct dhcp_msg {
    uint8_t opcode;
    uint8_t hwtype;
    uint8_t hwsize;
    uint8_t hwops;
    
    uint32_t id;
    uint16_t elapsed;
    uint16_t flags;

    uint8_t client_ip[4];
    uint8_t server_ip[4];
    uint8_t next_server_ip[4];
    uint8_t relay_agent_ip[4];
    
    uint8_t client_mac[16];
    uint8_t server_hostname[64];

    uint8_t boot_file[128];
    uint8_t options[312];
};

static int create_dhcp_socket() {
    
}
