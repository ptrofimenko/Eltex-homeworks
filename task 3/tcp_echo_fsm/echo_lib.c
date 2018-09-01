void init (void *args) {
	
	data_type *data = (data_type *)args;
	data->sock = -1;
	data->sock = socket(AF_INET, SOCK_STREAM, data->protocol);
	if (data->sock < 0) {
		perror("socket() failed");
	}
	data->serveraddr.sin_family = AF_INET;
	data->serveraddr.sin_port = htons(PORT);
	data->serveraddr.sin_addr.s_addr = htonl(data->addr);
	
	data->cl_cnt = 0;
}

void print_time(time_t timer) {
	//struct tm time_struct;
	//time_struct = localtime(&timer);
	char s[40] = {0};
	strftime(s, 40, "%d.%m.%Y %H:%M:%S, %A", localtime(&timer));
	printf("New request: %s\n", s);
}

void* echo(void *args) {
	data_type *data = (data_type *)args;
	char message[100];
	
	int state = TAKE_REQ;
	int sig;
	struct { long type; time_t timer; } request;
	
	while(1) {
		switch(state) {
			case TAKE_REQ:
				sig = msgrcv(data->mq_id, &request, sizeof(request), 0, 0);
				if (sig <= 0) {
					printf("Taking request failed\n");
					state = TAKE_REQ;
				}
				else {
					state = RCV_STR;
				}
			case RCV_STR:
				sig = recv(request.type, message, sizeof(message), 0);
				if (sig < 0) {
					printf("Recive string failed\n");
					state = TAKE_REQ;
				}
				else {
					state = CHANGE_AND_SEND;
				}
			case CHANGE_AND_SEND:
				message[0] = 'a';
				message[strlen(message) - 2] = '1';
				sig = send(request.type, message, sizeof(message), 0);
				if (sig < 0) {
					printf("Send string failed\n");
					state = TAKE_REQ;
				}
				else {
					state = TAKE_REQ;
				}
		}
	}
	
	return 0;
}


