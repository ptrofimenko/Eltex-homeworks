#include "echo_lib.h"
  
/*
    Generic checksum calculation function
*/
 
int main (void)
{
    
    /*send socket initialization*/
    int s = socket (AF_PACKET, SOCK_RAW, ETH_P_ALL);
     
    if(s == -1)
    {
        perror("Failed to create raw socket");
        exit(1);
    }
     
    int f = 1;
    setsockopt(s, IPPROTO_IP, IP_HDRINCL, &f, sizeof(f)); 
     
    /*Датаграмма для представления пакета*/
    char datagram[4096] , source_ip[32], dest_ip[32] , *data , *pseudogram;
     
    memset (datagram, 0, 4096);
    struct ethhdr *ethh = (struct ethhdr *) datagram; 
     
    //IP загловок
    struct iphdr *iph = (struct iphdr *) (datagram + sizeof (struct ethhdr));
     
    //UDP заголовок
    //struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip));
    struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct iphdr) + sizeof (struct ethhdr));
     
    struct sockaddr_ll sock_addr;
    struct pseudo_header psh;
     
    //Data part
    data = datagram + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data , "hhhh\0");
     
    strcpy(source_ip , "192.168.2.2");
    strcpy(dest_ip , "192.168.2.1");
   
	sock_addr.sll_family = AF_PACKET;
	sock_addr.sll_protocol = htons(ETH_P_IP);
	sock_addr.sll_ifindex = if_nametoindex("enp0s8");
	
    sock_addr.sll_addr[0] = 0x08;
    sock_addr.sll_addr[1] = 0x00;
    sock_addr.sll_addr[2] = 0x27;
    sock_addr.sll_addr[3] = 0x35;
    sock_addr.sll_addr[4] = 0x40;
    sock_addr.sll_addr[5] = 0x5E;
    sock_addr.sll_addr[6] = 0x00;
    sock_addr.sll_addr[7] = 0x00;
    //Заполнение ETH заголовка
    //mac получателя
    ethh->h_dest[0] = 0x08;
    ethh->h_dest[1] = 0x00;
    ethh->h_dest[2] = 0x27;
    ethh->h_dest[3] = 0x35;
    ethh->h_dest[4] = 0x40;
    ethh->h_dest[5] = 0x5E;
    //mac отправителя
    ethh->h_source[0] = 0x08;
    ethh->h_source[1] = 0x00;
    ethh->h_source[2] = 0x27;
    ethh->h_source[3] = 0x73;
    ethh->h_source[4] = 0xAC;
    ethh->h_source[5] = 0x54;
    
    ethh->h_proto = htons(ETH_P_IP);
    //Заполнение IP заголовка
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data);
    //iph->tot_len = sizeof (struct ethhdr) + sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data);
    
    iph->id = htons (54321); //Id of this packet
    printf("id = %d\n", ntohs(iph->id));
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;      //Set to 0 before calculating checksum
    iph->saddr = inet_addr ( source_ip );    //Spoof the source ip address
    iph->daddr = inet_addr ( dest_ip );
    printf("tot_len = %d\n", iph->tot_len);
     
    //Вычисление контрольной суммы
    iph->check = csum ((unsigned short *) datagram, iph->tot_len);
     
    //Заполнение UDP загловка
    udph->source = htons (6667);
    udph->dest = htons (3435);
    udph->len = htons(8 + strlen(data)); //tcp header size
    udph->check = 0; //leave checksum 0 now, filled later by pseudo header
     
    psh.source_address = inet_addr( source_ip );
    psh.dest_address = inet_addr ( dest_ip );
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr) + strlen(data) );
     
    int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
    pseudogram = malloc(psize);
     
    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + strlen(data));
     
    udph->check = csum( (unsigned short*) pseudogram , psize);
    
    
    struct sockaddr_ll sockstr;
    sockstr.sll_family = AF_PACKET;
    sockstr.sll_protocol = htons(ETH_P_ALL);
   
    int socklen = (socklen_t) sizeof(sockstr);

    if (bind(s, (struct sockaddr*) &sockstr, socklen) == -1) {
        perror("bind");
    }
    char msg[MSG_SIZE];
    ssize_t msglen; 
    //unsigned int sizeaddr = sizeof(sock_addr);
    
    struct ifreq ifr;
    strcpy(ifr.ifr_name, "enp0s8");
    ifr.ifr_flags |= IFF_PROMISC;
    if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0) {
      perror("Unable to set promiscious mode for device");
      close(s);
      exit(-1);
    }
    
	/*echo part*/
    while (1)
    {
        char input[100];
        fgets(input, 100, stdin);
        strcpy(data, input);
        
        
        strcpy(source_ip , "192.168.2.2");
     
		 
	iph->tot_len = htons(sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data)); 
	//iph->tot_len = htons(sizeof (struct ethhdr) + sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data));
	//iph->tot_len = 0x0030;
	printf("tot_len = %d\n", iph->tot_len);
	iph->check = 0;
	//iph->check = csum ((unsigned short *) datagram, iph->tot_len);
	iph->check = csum ((unsigned short*)iph, sizeof(struct iphdr));
	udph->source = htons (6667);
	// udph->source = htons (80);
	udph->dest = htons (3435);
	//udph->dest = htons(80);
	udph->len = htons(8 + strlen(data)); //tcp header size
	udph->check = 0; //leave checksum 0 now, filled later by pseudo header
     
	psh.source_address = inet_addr( source_ip );
	psh.dest_address = inet_addr ( dest_ip );
	psh.placeholder = 0;
	psh.protocol = IPPROTO_UDP;
	psh.udp_length = htons(sizeof(struct udphdr) + strlen(data) );
     
	int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
	pseudogram = malloc(psize);
     
	memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
	memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + strlen(data));
     
	udph->check = csum( (unsigned short*) pseudogram , psize);
     
	
        printf("to send:%s\nsize: %d\n", data, strlen(data));
        
        
        //Send the packet
        if (sendto (s, datagram, sizeof(struct ethhdr) + sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data) ,  0, (struct sockaddr *) &sock_addr, sizeof (sock_addr)) < 0)
        {
            perror("sendto failed");
        }
        //Data send successfully
        else
        {
            printf("%s\n", data);
            printf ("Packet Send. Length : %d \n" , iph->tot_len);
        }
        struct udphdr *udp_check;
        struct ethhdr *eth_check;
        struct iphdr *ip_check;
        //while(udp_check->dest != htons(6667)) {
		while(1) {	
			if ((msglen = recvfrom (s, msg, MSG_SIZE,  0, 0, 0)) < 0)
			{
				perror("recvfrom failed");
			}
			//udp_check = (struct udphdr *)(msg + sizeof(struct ethhdr) + sizeof(struct iphdr));
			
			eth_check = (struct ethhdr *)(msg);
			//printf("%d\n", ntohs(udp_check->dest));
			printf("h_proto = %d\n", ntohs(eth_check->h_proto));
			if(eth_check->h_proto == htons(ETH_P_IP)) {
				ip_check = (struct iphdr *)(msg + sizeof(struct iphdr));
				printf("ip_proto = %d\n", ip_check->protocol);
				//if(ip_check->protocol == IPPROTO_UDP) {
					udp_check = (struct udphdr *)(msg + sizeof(struct ethhdr) + sizeof(struct iphdr));
					printf("udp_dest = %d\n", ntohs(udp_check->dest));
					if(udp_check->dest == htons(6667)) {
						break;
					}
				//}
			}
        }
        printf("msglen:%d\nmsg: %s\n", msglen, msg + HEADER_SIZE);
        
       
        
       
    }
     
    return 0;
}

