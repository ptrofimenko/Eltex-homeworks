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

#define LIMIT 10

#define ERR(...) fprintf( stderr, "\7" __VA_ARGS__ ), exit( EXIT_FAILURE )


 
#define IOC_MAGIC    'k' 
#define IOCTL_GET_NFILTERS _IOR( IOC_MAGIC, 0, int)
#define IOCTL_GET_STAT _IOR( IOC_MAGIC, 1, char *)
#define IOCTL_GET_STATLEN _IOR( IOC_MAGIC, 3, int)
#define IOCTL_SEND_FILTER _IOW( 'h', 2, filter_struct )
#define DEVPATH "/proc/my_firewall"

typedef struct {
	int type;
	int transport;
	int port;
	int disable_enable;
	char *ip;
} filter_struct;

void wrong_format();
int type(int, char **, filter_struct *);
int ip(int, char **, filter_struct *);
int port(int, char **, filter_struct *);
int disable_enable(int, char **, filter_struct *);
int send_filter(filter_struct *fil);

#endif 
