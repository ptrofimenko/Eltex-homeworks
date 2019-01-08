#include "echo_lib.h"
#include "echo_lib.c"

int main() {
	printf("***CLIENT***\n");
	data_type data;

	data.protocol = IPPROTO_TCP;
	data.addr = INADDR_LOOPBACK;
	
	init((void *)&data);
	
	if((connect(data.sock, (struct sockaddr *)&data.serveraddr, sizeof(data.serveraddr))) < 0) {
		perror("connect() failed");
		return 0;
	}
	
	char message[100];
	fgets(message, 100, stdin);
	for(int i = 0; i < 10; i++) {
		sleep(1);
		send(data.sock, message, sizeof(message), 0);
		recv(data.sock, message, sizeof(message), 0);
		printf("%s", message);
	}
}
