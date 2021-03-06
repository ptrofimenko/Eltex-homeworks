#ifndef __TUFILTER_H__
#define __TUFILTER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>		//struct in_addr
#include <sys/socket.h>
#include <arpa/inet.h>

#define ERR(...) fprintf( stderr, "\7" __VA_ARGS__ ), exit( EXIT_FAILURE )

typedef struct {
	int type;
	int transport;
	int port;
	int disable_enable;
	struct in_addr ip;
} filter_struct;

void wrong_format();
int type(int, char **, filter_struct *);
int ip(int, char **, filter_struct *);
int port(int, char **, filter_struct *);
int disable_enable(int, char **, filter_struct *);
int send_filter(filter_struct *fil, int desc_proc);

#endif 
