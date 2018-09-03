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
#include <sys/epoll.h>

#define PORT 3440

typedef struct {
	int sock;
	int cl_socks[2000];
	int cl_cnt;
	int real_cl_cnt;
	int protocol;
	int addr;
	struct sockaddr_in serveraddr;
	int efd;
	struct epoll_event event;
	int threads;
	int maxev;
	int so_p_th;
	pthread_t th;
} data_type;

void init (void *args);
void print_time (time_t timer);
void* echo (void *args);

#endif	
