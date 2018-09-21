#include "echo_lib.h"
#include "echo_lib.c"

int main() {
	printf("***SERVER***\n");
	data_type data;
	
	pthread_t th;

	data.protocol = IPPROTO_UDP;
	data.addr = INADDR_ANY;
	int r_port = RCV_PORT;
	int s_port = SND_PORT;
	init((void *)&data, r_port, s_port);
	if(bind(data.sock, (struct sockaddr *)&data.serveraddr, sizeof(data.serveraddr)) < 0) {
		perror("bind() failed");
	}
	
	int ports[100];
	data.cl_cnt = 0;
	int i = 0;
	int fl_connected = 0;
	unsigned int client_addr_size;
	
	for(;;) {
		client_addr_size = sizeof(data.client_addr);					
		data.last_addr = 0;
		memset (data.message, 0, 100);
		if (recvfrom(data.sock, data.message, sizeof(data.message), 0, 
		(struct sockaddr *) &data.client_addr, &client_addr_size) < 0) {
			perror("recvfrom");
		}
		
		data.last_addr = data.client_addr.sin_port;
		printf("received msg: %s\nsize: %ld\n", data.message, strlen(data.message));
		//printf("%d | %d\n", data.last_addr, data.client_addr.sin_addr.s_addr);
		//printf("%d\n", data.client_addr.sin_port);
		for(i = 0; i < data.cl_cnt; i++) {
			if (data.client_addr.sin_port == ports[i]) {
				fl_connected = 1;
			}
			printf("fl_connected = %d\n", fl_connected); 
		}
		if (!fl_connected) {
			if(pthread_create(&th, NULL, sender, (void *)&data) != 0) {
				perror("pthread_create() failed");
			}
			if(pthread_detach(th) != 0) {
				perror("pthread_detach() failed");
			}
			ports[data.cl_cnt] = data.client_addr.sin_port;
			data.cl_cnt++;
			printf("New client\n"); 
		}
		fl_connected = 0;
		sleep(1);
	}
	return 0;
}
