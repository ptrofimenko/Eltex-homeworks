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

int main() {
	data_type data;
	data.ipckey = ftok("/tmp/foo1", 42);
	data.mq_id = msgget(data.ipckey, 0);
	msgctl(data.ipckey, IPC_RMID, 0);
	return 0;
}
