#include "echo_lib.h"
#include "echo_lib.c"

#define SERVANTS 3

int main() {
	printf("***SERVER***\n");
	data_type data;
	pthread_t th;
	time_t timer;
	
	struct { long type; time_t timer; } request;
 
    /* Generate the ipc key */
    data.ipckey = ftok("/tmp/foo1", 42);
     
    /* Set up the message queue */
    data.mq_id = msgget(data.ipckey, IPC_CREAT | 0666);
	

	data.protocol = IPPROTO_TCP;
	data.addr = INADDR_ANY;
	
	init((void *)&data);
	if(bind(data.sock, (struct sockaddr *)&data.serveraddr, sizeof(data.serveraddr)) < 0) {
		perror("bind() failed");
	}
	
	for(int i = 0; i < SERVANTS; i++) {
		if(pthread_create(&th, NULL, echo, (void *)&data) != 0) {
			perror("pthread_create() failed");
		}
		if(pthread_detach(th) != 0) {
			perror("pthread_detach() failed");
		}
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
		
		recv(data.cl_socks[data.cl_cnt], &timer, sizeof(timer), 0);
		print_time(timer);
		request.type = data.cl_socks[data.cl_cnt];
		request.timer = timer;
		msgsnd(data.mq_id, &request, sizeof(request), 0);
		
		data.cl_cnt++;
	}
	return 0;
}
