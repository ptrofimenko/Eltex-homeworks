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
	char s[40] = {0};
	strftime(s, 40, "%d.%m.%Y %H:%M:%S, %A", localtime(&timer));
	printf("New client connected: %s\n", s);
}

void* echo(void *args) {
	data_type *data = (data_type *)args;
	char str[100];
	time_t timer;
	int rcvd;
	
	struct epoll_event *events;
	events = calloc (data->maxev, sizeof data->event); 
	int events_cnt = 0;
	for(;;) {
		
		events_cnt = epoll_wait (data->efd, events, data->maxev, -1);
		for (int i = 0; i < events_cnt; i++) {	
			if(data->sock == events[i].data.fd) {
				if((data->cl_socks[data->cl_cnt] = accept(data->sock, NULL, NULL)) < 0) {
					perror("accept() failed");
					close(data->sock);
					return 0;
				}
		
				recv(data->cl_socks[data->cl_cnt], &timer, sizeof(timer), 0);
				print_time(timer);
				
				data->event.data.fd = data->cl_socks[data->cl_cnt];
				data->event.events = EPOLLIN;
	
				if(epoll_ctl (data->efd, EPOLL_CTL_ADD, data->cl_socks[data->cl_cnt], &data->event) == -1) {
					perror("epoll_ctl");
					return 0;
				}
		
				data->cl_cnt++;
				data->real_cl_cnt++;
				
				if(data->real_cl_cnt > data->threads * data->so_p_th) {
					data->threads++;
					if(pthread_create(&data->th, NULL, echo, (void *)&data) != 0) {
						perror("pthread_create() failed");
					}		
					if(pthread_detach(data->th) != 0) {
						perror("pthread_detach() failed");
					}
					printf("Count of clients = %d... New thread started\n", data->real_cl_cnt);
					
				}	
			}
			else {
				rcvd = recv(events[i].data.fd, &str, sizeof(str), 0);
				if (rcvd > 0) {
					str[0] = 'a';
					str[strlen(str) - 2] = '1';
					send(events[i].data.fd, &str, sizeof(str), 0);
				}
				else { 
					data->real_cl_cnt--;
					close(events[i].data.fd); 
					printf("Client disconected\n"); 
				}
			}
		}
	}
	return 0;
}		
	


