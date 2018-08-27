#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>

typedef struct {	
		key_t ipckey;
		int mq_id;
		long type;
    } data_type;

void* receiver(void *);

int main() {
	printf("***CLIENT***\n");
	data_type data;
	
	pthread_t th;
    struct { long type; char text[100]; } mymsg;
    struct { long type; int cl_id; } type_msg;
    
        
	type_msg.cl_id = 0;
    /* Generate the ipc key */
    data.ipckey = ftok("/tmp/foo1", 42);
     
    /* Set up the message queue */
    data.mq_id = msgget(data.ipckey, 0);
     
    /* Send a message */
    type_msg.type = 500;
    type_msg.cl_id = 1;
    msgsnd(data.mq_id, &type_msg, sizeof(type_msg), 0);
    msgrcv(data.mq_id, &type_msg, sizeof(type_msg), 501, 0);
    
    data.type = type_msg.cl_id;
    printf("type = %ld\n", data.type);
    if(pthread_create(&th, NULL, receiver, (void *)&data) != 0) {
		perror("pthread_create() failed");
	}
	if(pthread_detach(th) != 0) {
		perror("pthread_detach() failed");
	}
	mymsg.type = data.type;
    while(1) {
		fgets(mymsg.text, 100, stdin);
		msgsnd(data.mq_id, &mymsg, sizeof(mymsg), 0);
	}
    return 0; 
}

void* receiver(void *args) {
	data_type *data = (data_type *)args;
	struct { long type; char text[100]; } mymsg;
	memset(mymsg.text, 0, 100);
	while(1) {
		msgrcv(data->mq_id, &mymsg, sizeof(mymsg), data->type + 100, 0);
		printf("%s", mymsg.text);
	}
	return 0;
}
