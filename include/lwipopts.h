#ifndef __lwipopts_h__
#define __lwipopts_h__

#define NO_SYS      1
#define LWIP_SOCKET 0 // required when NO_SYS = 1

#define MEM_LIBC_MALLOC     0
#define MEM_ALIGNMENT       4
#define MEM_SIZE            4000
#define MEMP_NUM_TCP_SEG    32
#define MEMP_NUM_ARP_QUEUE  10
#define PBUF_POOL_SIZE      24

// TCP settings
#define TCP_MSS 1460 // max TCP segment size
#define TCP_WND TCP_MSS * 8 // max window size is 8 segments
#define TCP_SND_BUF TCP_WND // should be the same as the window
#define TCP_SND_QUEUELEN (4 * TCP_SND_BUF + (TCP_MSS - 1)) / TCP_MSS

// enable all the networking >:) (for now)
#define LWIP_ARP                  1
#define LWIP_ETHERNET             1
#define LWIP_ICMP                 1
#define LWIP_RAW                  1
#define LWIP_CHKSUM_ALGORITHM     3
#define LWIP_DHCP                 1
#define LWIP_IPV4                 1
#define LWIP_TCP                  1
#define LWIP_UDP                  1
#define LWIP_DNS                  1
#define LWIP_TCP_KEEPALIVE        1
#define LWIP_NETIF_TX_SINGLE_PBUF 1
#define LWIP_DHCP_DOES_ACD_CHECK  0
#define DHCP_DOES_ARP_CHECK       0
#define LWIP_DEBUG                1

// network interface settings
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK 1
#define LWIP_NETIF_HOSTNAME 1
#define LWIP_NETCONN 0 // allows NO_SYS = 1

// disable output
#define SYS_STATS 0
#define MEMP_STATS 0
#define LINK_STATS 0

#endif