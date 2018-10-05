#ifndef ECHO_LIB_H
#define ECHO_LIB_H

#include <stdio.h>
#include <pthread.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <sys/socket.h>    
#include <stdlib.h> 
#include <errno.h> 
#include <netinet/udp.h>   
#include <netinet/ip.h>    
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#include <net/if.h>

#define RCV_PORT 3435
#define SND_PORT 3436
#define ADDR_TO_BIND "192.168.2.1"
#define PORT_TO_BIND 3435

#define MSG_SIZE 256
#define HEADER_SIZE (sizeof(struct iphdr) + sizeof(struct udphdr))

typedef struct {
	int sock;
	int send_sock;
	int cl_cnt;
	int protocol;
	int addr;
	int last_addr;
	char message[100];	
	struct sockaddr_in serveraddr;
	struct sockaddr_in client_addr;
} data_type;

struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};

void init (void *args, int r_port, int s_port);
void* sender (void *args);

#endif	
