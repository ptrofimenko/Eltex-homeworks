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


