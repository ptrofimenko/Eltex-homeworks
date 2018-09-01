#ifndef ECHO_LIB_H
#define ECHO_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define PORT 3435
#define TAKE_REQ 1351
#define RCV_STR 1352
#define CHANGE_AND_SEND 1353

typedef struct {
	int sock;
	int cl_socks[100];
	int cl_cnt;
	int protocol;
	int addr;
	key_t ipckey;
	int mq_id;
	struct sockaddr_in serveraddr;
} data_type;

void init (void *args);
void print_time (time_t timer);
void* echo (void *args);

#endif	
