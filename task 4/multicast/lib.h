#ifndef ECHO_LIB_H
#define ECHO_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3435
#define GROUP "239.0.0.1"

typedef struct {
	int sock;
	int protocol;
	int addr;
	int addrlen;
	char message[100];	
	struct sockaddr_in serveraddr;
	struct sockaddr_in client_addr;
} data_type;

void init (void *args);
void* sender (void *args);

#endif	
