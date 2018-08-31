void init (void *args) {
	
	data_type *data = (data_type *)args;
	data->sock = -1;
	data->sock = socket(AF_INET, SOCK_DGRAM, data->protocol);
	if (data->sock < 0) {
		perror("socket() failed");
	}
	data->serveraddr.sin_family = AF_INET;
	data->serveraddr.sin_port = htons(PORT);
	data->serveraddr.sin_addr.s_addr = htonl(data->addr);
	
	data->cl_cnt = 0;
}

void* sender (void *args) {
	data_type *data = (data_type *)args;
	struct sockaddr_in client_addr = data->client_addr;
	for(;;) {
		if (data->last_addr == client_addr.sin_port) {
			data->message[0] = 'a';
			data->message[strlen(data->message) - 2] = '1';
			sendto(data->sock, data->message, sizeof(data->message), 0, (struct sockaddr *) &client_addr, sizeof(client_addr));
			data->last_addr = 0;
		}
	}
}
		
