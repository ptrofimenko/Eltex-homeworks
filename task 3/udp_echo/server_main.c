#include "echo_lib.h"
#include "echo_lib.c"

int main() {
	printf("***SERVER***\n");
	data_type data;
	

	data.protocol = IPPROTO_UDP;
	data.addr = INADDR_ANY;
	
	init((void *)&data);
	if(bind(data.sock, (struct sockaddr *)&data.serveraddr, sizeof(data.serveraddr)) < 0) {
		perror("bind() failed");
	}
	struct sockaddr_in client_addr;
	unsigned int client_addr_size;
	char message[100];	
	for(;;) {
		client_addr_size = sizeof(client_addr);					
		recvfrom(data.sock, message, sizeof(message), 0, (struct sockaddr *) &client_addr, &client_addr_size);	
		message[0] = 'a';
		message[strlen(message) - 2] = '1';
		sendto(data.sock, message, sizeof(message), 0, (struct sockaddr *) &client_addr, sizeof(client_addr));	
		sleep(1);
	}
	return 0;
}
