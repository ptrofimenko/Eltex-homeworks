void init (void *args, int r_port, int s_port) {
	
	data_type *data = (data_type *)args;
	data->sock = -1;
	data->sock = socket(AF_INET, SOCK_DGRAM, data->protocol);
	if (data->sock < 0) {
		perror("socket() failed");
	}
	data->serveraddr.sin_family = AF_INET;
	data->serveraddr.sin_port = htons(RCV_PORT);
	data->serveraddr.sin_addr.s_addr = htonl(data->addr);
	
	/*data->send_sock = -1;
	data->send_sock = socket(AF_INET, SOCK_DGRAM, data->protocol);
	if (data->sock < 0) {
		perror("socket() failed");
	}
	data->serveraddr.sin_family = AF_INET;
	data->serveraddr.sin_port = htons(PORT);
	data->serveraddr.sin_addr.s_addr = htonl(data->addr);*/
	
	data->cl_cnt = 0;
}

void* sender (void *args) {
	data_type *data = (data_type *)args;
	struct sockaddr_in client_addr = data->client_addr;
	for(;;) {
		if (data->last_addr == client_addr.sin_port) {
			data->message[0] = 'a';
			data->message[strlen(data->message) - 2] = '1';
			data->message[strlen(data->message) - 1] = '\0';
			printf("to send: %s\n", data->message);
			sendto(data->sock, data->message, sizeof(data->message), 0, (struct sockaddr *) &client_addr, sizeof(client_addr));
			data->last_addr = 0;
			memset (data->message, 0, 100);
		}
		sleep(1);
	}
}

/*функция подсчёта контрольной суммы*/
unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum = 0;
    while(nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if(nbytes == 1) {
        oddbyte = 0;
        *((u_char*)&oddbyte) =* (u_char*)ptr;
        sum += oddbyte;
    }
 
    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;
     
    return(answer);
}
		
