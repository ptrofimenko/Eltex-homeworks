#ifndef __TUFILTER_H__
#define __TUFILTER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define SHOW 23
#define SET 24
#define TCP 33
#define UDP 34
#define DIS 5
#define EN 6

#define ERR(...) fprintf( stderr, "\7" __VA_ARGS__ ), exit( EXIT_FAILURE )

typedef struct _RETURN_STRING { 
   char buf[ 160 ]; 
} RETURN_STRING; 
 
#define IOC_MAGIC    'h' 
#define IOCTL_GET_STRING _IOR( IOC_MAGIC, 1, RETURN_STRING ) 
#define DEVPATH "/proc/my_firewall"

typedef struct {
	int type;
	int transport;
	char *ip;
	int port;
	int disable_enable;
} filter_struct;

void wrong_format();
int type(int, char **, filter_struct *);
int ip(int, char **, filter_struct *);
int port(int, char **, filter_struct *);
int disable_enable(int, char **, filter_struct *);

#endif 
