void init (void *args) {
	data_type *data = (data_type *)args;
	char error[PCAP_ERRBUF_SIZE];
	
	//Получение имени устройства для захвата пакетов
	data->dev_name = pcap_lookupdev(error);
	if(data->dev_name == NULL) {
		fprintf(stderr, "%s\n", error);
	}
	printf("device name: %s\n", data->dev_name);
	
	pcap_lookupnet(data->dev_name, &data->ip, &data->mask, NULL);
	
	
	//Получние идентификатора устройства для захвата пакетов
	data->dev_id = pcap_open_live(data->dev_name, 1518, 1, -1, error);
	
	/*Раскоментить для захвата пакетов на 127.0.0.1*/
	/*pcap_lookupnet("lo", &data->ip, &data->mask, NULL);
	data->dev_id = pcap_open_live("lo", 1518, 1, -1, error);*/
	
	if(data->dev_id == NULL) {
		fprintf(stderr, "%s\n", error);
	}
	
	//Составление выражения фильтрации
	if(pcap_compile(data->dev_id, &data->fp, data->filter, 0, data->ip) == -1) {
		fprintf(stderr, "Error calling pcap_compile\n");
		exit(1);
	}
	
	//Применение фильтра
	if(pcap_setfilter(data->dev_id, &data->fp) == -1) {
		fprintf(stderr, "Error setting filter\n");
		exit(1);
	}
	
	  
}

void callback(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) 
{  
	static int count = 0; 

	printf("Packet Count: %d\n", ++count);             
	printf("Recieved Packet Size: %d\n", pkthdr->len); 
	printf("Payload:\n");                             
	for(int i = 0; i < pkthdr->len; i++) { 
		if(isprint(packet[i]))            
			printf("%c",packet[i]);       
		else 
			printf(".");     
		if((i % 16 == 0 && i != 0) || i == pkthdr->len - 1) 
			printf("\n"); 
	}
}
