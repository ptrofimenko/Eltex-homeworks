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

void* echo(void *args) {
	data_type *data = (data_type *)args;
	int client_socket = data->cl_cnt;
	char message[100];
	
	while(1) {
		
		if(recv(data->cl_socks[client_socket], message, sizeof(message), 0) <= 0) {
				return 0;
		}
		printf("%s", message);
		message[0] = 'a';
		message[strlen(message) - 2] = '1';
		printf("%s", message);
		send(data->cl_socks[client_socket], message, sizeof(message), 0);
		
		sleep(1);
		
	}	
	return 0;
}
