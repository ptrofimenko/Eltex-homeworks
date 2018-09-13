#include "lib.h"
#include "lib.c"

int main() {
	printf("***SERVER***\n");
	data_type data;
	bzero((char *)&data.serveraddr, sizeof(data.serveraddr));
	data.protocol = IPPROTO_UDP;
	data.addr = inet_addr(GROUP);
	data.addrlen = sizeof(data.serveraddr);
	
	data.serveraddr.sin_addr.s_addr = inet_addr(GROUP);
	
	
	init((void *)&data);
	int size = 0;
	printf("starting multicast\n");
	for(;;) {
		time_t t = time(0);
		sprintf(data.message, "time is %-24.24s", ctime(&t));
		printf("sending: %s\n", data.message);
		size = sendto(data.sock, data.message, sizeof(data.message), 0, (struct sockaddr *) &data.serveraddr, sizeof(data.serveraddr));
		if (size < 0) {
			perror("sendto");
			exit(1);
		}
		sleep(3);
	}
	return 0;
}
