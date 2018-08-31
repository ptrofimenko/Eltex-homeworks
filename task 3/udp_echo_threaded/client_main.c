#include "echo_lib.h"
#include "echo_lib.c"

int main() {
	printf("***CLIENT***\n");
	data_type data;

	data.protocol = IPPROTO_UDP;
	data.addr = INADDR_LOOPBACK;
	
	init((void *)&data);
	
	unsigned int server_addr_size = sizeof(data.serveraddr);
	char message[100];
	while(1) {
		fgets(message, 100, stdin);
		sendto(data.sock, message, sizeof(message), 0, 
		(struct sockaddr *)&data.serveraddr, sizeof(data.serveraddr));
		recvfrom(data.sock, message, sizeof(message), 0, 
		(struct sockaddr *) &data.serveraddr, &server_addr_size);
		printf("%s", message);
	}
}
