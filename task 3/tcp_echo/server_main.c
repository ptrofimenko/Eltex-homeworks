#include "echo_lib.h"
#include "echo_lib.c"

int main() {
	printf("***SERVER***\n");
	data_type data;
	pthread_t th;

	data.protocol = IPPROTO_TCP;
	data.addr = INADDR_ANY;
	
	init((void *)&data);
	if(bind(data.sock, (struct sockaddr *)&data.serveraddr, sizeof(data.serveraddr)) < 0) {
		perror("bind() failed");
	}
	for(;;) {
		if(listen(data.sock, 1) < 0) {
			perror("listen() failed");
			close(data.sock);
			return 0;
		}
		if((data.cl_socks[data.cl_cnt] = accept(data.sock, NULL, NULL)) < 0) {
			perror("accept() failed");
			close(data.sock);
			return 0;
		}
		if(pthread_create(&th, NULL, echo, (void *)&data) != 0) {
			perror("pthread_create() failed");
		}
		if(pthread_detach(th) != 0) {
			perror("pthread_detach() failed");
		}
		sleep(1);
		data.cl_cnt++;
	}
	return 0;
}
