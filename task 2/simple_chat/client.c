#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3434

typedef struct {
	int sock;
	struct sockaddr_in serveraddr;
} data_type;

void* receiver(void *);
void init (void *);

int main() {
	data_type data;
	pthread_t th;
	/*Инциализация сокета*/
	init((void *)&data);
	/*Установка соединения с сервером*/
	if((connect(data.sock, (struct sockaddr *)&data.serveraddr, sizeof(data.serveraddr))) < 0) {
		perror("connect() failed");
		return 0;
	}
	char *buf;
	buf = malloc(sizeof(buf) * 1024);
	/*Создание потока, ждущего новые сообщения от сервера*/
	if(pthread_create(&th, NULL, receiver, (void *)&data) != 0) {
		perror("pthread_create() failed");
	}
	if(pthread_detach(th) != 0) {
		perror("pthread_detach() failed");
	}
	
	/*Ввод и отправка новых сообщений серверу*/
	while(1) {
		fgets(buf, 1024, stdin);
		send(data.sock, buf, sizeof(buf), 0);
		//printf("%s", buf);
	}
	close(data.sock);	
	return 0;
}

void init(void *args) {
	data_type *data = (data_type *)args;
	data->sock = -1;
	data->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (data->sock < 0) {
		perror("socket() failed");
		//return 0;
	}
	data->serveraddr.sin_family = AF_INET;
	data->serveraddr.sin_port = htons(PORT);
	data->serveraddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

void* receiver(void *args) {
	data_type *data = (data_type *)args;
	char *buf;
	buf = malloc(sizeof(buf) * 1024);
	while(1) {
		if(recv(data->sock, buf, sizeof(buf), 0) <= 0) {
			break;
		}
		printf("%s", buf);
	}
	return 0;
}

