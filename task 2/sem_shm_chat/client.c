#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>

typedef struct {	
		key_t ipckey;
		char *shared;
		int flag;
		char current_msg[100];
} data_type;

void* receiver(void *);

int main() {
	
	data_type data;
	int shmid;
	char text[100];
	pthread_t th;
	int semid;
    struct sembuf sem[2];
    
    data.ipckey = ftok("/tmp/foo", 42);
    shmid = shmget(data.ipckey, sizeof(char) * 100, 0);
	data.shared = shmat(shmid, (void *) 0, 0);
	semid = semget(data.ipckey, 1, 0);

    if(pthread_create(&th, NULL, receiver, (void *)&data) != 0) {
		perror("pthread_create() failed");
	}
	if(pthread_detach(th) != 0) {
		perror("pthread_detach() failed");
	}
	data.flag = 0;
    sem[0].sem_num = 0;
    sem[1].sem_num = 0;
    sem[0].sem_flg = SEM_UNDO; 
    sem[1].sem_flg = SEM_UNDO; 
    while(1) {
		fgets(text, 100, stdin);
		sem[0].sem_op = 0; /* Wait for zero */
        sem[1].sem_op = 1; /* Add 1 to lock it*/
        semop(semid, sem, 2);
		
		data.flag++;
		strcpy(data.current_msg, text);
		strcpy(data.shared, text);
		data.flag--;
		
		sem[0].sem_op = -1; /* Decrement to unlock */
        semop(semid, sem, 1);
	}
    return 0; 
}

void* receiver(void *args) {
	
	data_type *data = (data_type *)args;
	strcpy(data->current_msg, data->shared);
	
	while(1) {
		if((strcmp(data->current_msg, data->shared) != 0) && data->flag == 0) {
			printf("%s", data->shared);
			strcpy(data->current_msg, data->shared);
		}
	}
	return 0;
}
