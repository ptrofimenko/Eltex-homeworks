#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3434

typedef struct {
	int sock;
	struct sockaddr_in serveraddr;
	int client_cnt;
	int last_sender;
	char *message;
	int msg_cnt;
	int* cl_socs;
} data_type;

void* sender(void *);
void* receiver(void *);
void init (void *);

int main() {
	printf("***SERVER***\n");
	data_type data;
	pthread_t th;
	/*инициализация сокета*/
	init((void *)&data);
	/*в бесконечном цикле ждём подключения новых клиентов*/
	while(1) {	
		if(listen(data.sock, 1) < 0) {
			perror("listen() failed");
			close(data.sock);
			return 0;
		}
		if((data.cl_socs[data.client_cnt] = accept(data.sock, NULL, NULL)) < 0) {
			perror("accept() failed");
			close(data.sock);
			return 0;
		}
		/*Создание потоков для обслуживания подключившегося клиента
		 * поток-sender при поступлении нового сообщения отправляет 
		 * его своему клиенту (если сообщение не от него) 
		 * поток-receiver ждёт сообщение от своего клиента */
		if(pthread_create(&th, NULL, sender, (void *)&data) != 0) {
			perror("pthread_create() failed");
		}
		if(pthread_detach(th) != 0) {
			perror("pthread_detach() failed");
		}
		if(pthread_create(&th, NULL, receiver, (void *)&data) != 0) {
			perror("pthread_create() failed");
		}
		if(pthread_detach(th) != 0) {
			perror("pthread_detach() failed");
		}
		sleep(1);
		data.client_cnt++;
	}
	
	printf("done");
	return 0;
}

void init (void *args) {
	data_type *data = (data_type *)args;
	data->sock = -1;
	data->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (data->sock < 0) {
		perror("socket() failed");
	}
	data->serveraddr.sin_family = AF_INET;
	data->serveraddr.sin_port = htons(PORT);
	data->serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(data->sock, (struct sockaddr *)&data->serveraddr, sizeof(data->serveraddr)) < 0) {
		perror("bind() failed");
	}
	
	data->client_cnt = 0;
	data->msg_cnt = 0;
	data->cl_socs = malloc(sizeof(data->cl_socs) * 1024);
}
	

void* sender(void *args) {
	data_type *data = (data_type *)args;
	int client_socket = data->client_cnt;
	int cur_msg_cnt = data->msg_cnt;
	while(1) {
		sleep(1);
		if(data->last_sender != client_socket && data->msg_cnt != cur_msg_cnt) { 
			if(data->cl_socs[client_socket] == -1) {
				return 0;
			}
			send(data->cl_socs[client_socket], data->message, sizeof(data->message), 0);
			cur_msg_cnt = data->msg_cnt;
		}
	}	
	return 0;
}

void* receiver(void *args) {
	data_type *data = (data_type *)args;
	int client_socket = data->client_cnt;
	data->message = malloc(sizeof(data->message) * 1024);
	while(1) {	
		if(recv(data->cl_socs[client_socket], data->message, sizeof(data->message), 0) <= 0) {
			data->cl_socs[client_socket] = -1;
			return 0;
		}
		printf("%s", data->message);
		data->last_sender = client_socket;
		data->msg_cnt++;
	}
	return 0;
}
