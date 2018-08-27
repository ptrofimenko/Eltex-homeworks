#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>

typedef struct {
	key_t ipckey;
	int mq_id;
	int client_cnt;
	int last_sender;
	int msg_cnt;
	char msg[100];
} data_type;

void* sender(void *);
void* receiver(void *);

int main() {
	printf("***SERVER***\n");
	data_type data;
    pthread_t th;
    data.msg_cnt = 0;
    memset(data.msg, 0, 100);
    
    struct { long type; int cl_id; } type_msg;
 
    /* Generate the ipc key */
    data.ipckey = ftok("/tmp/foo1", 42);
     
    /* Set up the message queue */
    data.mq_id = msgget(data.ipckey, IPC_CREAT | 0666);
    data.client_cnt = 0;
    while(1) {
		data.client_cnt++;
		
		msgrcv(data.mq_id, &type_msg, sizeof(type_msg), 500, 0); 
		type_msg.type = 501;
		type_msg.cl_id = data.client_cnt;
		msgsnd(data.mq_id, &type_msg, sizeof(type_msg), 0);
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
		sleep(2);
	}
    return 0;
}

void* sender(void *args) {
	data_type *data = (data_type *)args;
	
	int cur_msg_cnt = data->msg_cnt;
	struct { long type; char msg[100]; } s_msg;
	memset(s_msg.msg, 0, 100);
	s_msg.type = data->client_cnt + 100;
	
	while(1) {
		if(cur_msg_cnt != data->msg_cnt && data->last_sender + 100 != s_msg.type) {
			strcpy(s_msg.msg, data->msg);
			msgsnd(data->mq_id, &s_msg, sizeof(s_msg), 0);
			
			cur_msg_cnt = data->msg_cnt;
			sleep(1);
			 
		}
	}	
	return 0;
}

void* receiver(void *args) {
	data_type *data = (data_type *)args;
	
	struct { long type; char msg[100]; } r_msg;
	
	memset(r_msg.msg, 0, 100);
	r_msg.type = data->client_cnt;
	
	while(1) {
		msgrcv(data->mq_id, &r_msg, sizeof(r_msg), r_msg.type, 0);
		data->last_sender = r_msg.type;
		
		strcpy(data->msg, r_msg.msg);
		data->msg_cnt++;
		printf("%s", r_msg.msg);
		sleep(1); 
	}
	return 0;
}

