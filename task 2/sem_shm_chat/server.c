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
		char text[100];
} data_type;

void* receiver(void *);

int main() {
	
	data_type data;
	int shmid;
	char *shared;
	int semid;
    struct sembuf sem[2];
    
    data.ipckey = ftok("/tmp/foo", 42);
    shmid = shmget(data.ipckey, sizeof(char) * 100, IPC_CREAT | 0666);
	shared = shmat(shmid, (void *) 0, 0);
    semid = semget(data.ipckey, 1, 0666 | IPC_CREAT);
    
    strcpy(shared, "");
    
    
    while(1) {
		fgets(data.text, 100, stdin);
		if (strcmp(data.text, "exit\n") == 0) {
			shmctl(shmid, IPC_RMID, 0);
			break;
		}
	}
    return 0; 
}


