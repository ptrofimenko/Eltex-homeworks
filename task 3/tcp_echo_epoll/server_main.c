#include "echo_lib.h"
#include "echo_lib.c"

#define MAXEVENTS 64
#define SOCK_PER_THREAD 100

int main() {
	printf("***SERVER***\n");
	data_type data;
	
	data.maxev = MAXEVENTS;
	data.so_p_th = SOCK_PER_THREAD;
	data.threads = 1;
	
	
	data.protocol = IPPROTO_TCP;
	data.addr = INADDR_ANY;
	
	init((void *)&data);
	if(bind(data.sock, (struct sockaddr *)&data.serveraddr, sizeof(data.serveraddr)) < 0) {
		perror("bind() failed");
	}
	
	data.efd = epoll_create1 (0);
	data.event.data.fd = data.sock;
	data.event.events = EPOLLIN;
	
	if(epoll_ctl (data.efd, EPOLL_CTL_ADD, data.sock, &data.event) == -1) {
		perror("epoll_ctl");
		return 0;
	}
	
	if(listen(data.sock, 1) < 0) {
			perror("listen() failed");
			close(data.sock);
			return 0;
	}
	
	echo(&data);
	
	return 0;
}
