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

#define PORT 3436

typedef struct {
	int sock;
	int cl_socks[100];
	int cl_cnt;
	int protocol;
	int addr;
	struct sockaddr_in serveraddr;
} data_type;

void init (void *args);
void* echo (void *args);

#endif	
