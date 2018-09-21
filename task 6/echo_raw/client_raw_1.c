#include "echo_lib.h"
#include "echo_lib.c"
  
/*
    Generic checksum calculation function
*/
 
int main (void)
{
    
    /*send socket initialization*/
    int s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
     
    if(s == -1)
    {
        perror("Failed to create raw socket");
        exit(1);
    }
     
    /*Датаграмма для представления пакета*/
    char datagram[4096] , source_ip[32] , *data , *pseudogram;
     
    memset (datagram, 0, 4096);
     
    //IP загловок
    struct iphdr *iph = (struct iphdr *) datagram;
     
    //UDP заголовок
    struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip));
     
    struct sockaddr_in sin;
    struct pseudo_header psh;
     
    //Data part
    data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data , "hhhh\0");
     
    strcpy(source_ip , "127.0.0.1");
     
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr ("127.0.0.1");
     
    //Заполнение IP заголовка
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data);
    iph->id = htonl (54321); //Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;      //Set to 0 before calculating checksum
    iph->saddr = inet_addr ( source_ip );    //Spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;
     
    //Вычисление контрольной суммы
    iph->check = csum ((unsigned short *) datagram, iph->tot_len);
     
    //Заполнение UDP загловка
    udph->source = htons (6667);
    udph->dest = htons (3435);
    udph->len = htons(8 + strlen(data)); //tcp header size
    udph->check = 0; //leave checksum 0 now, filled later by pseudo header
     
    psh.source_address = inet_addr( source_ip );
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = htons(sizeof(struct udphdr) + strlen(data) );
     
    int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
    pseudogram = malloc(psize);
     
    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + strlen(data));
     
    udph->check = csum( (unsigned short*) pseudogram , psize);
     
    /*receive socket initialization*/
    int raw_socket;
    struct sockaddr_in sockstr;
    socklen_t socklen;


    char msg[MSG_SIZE];
    ssize_t msglen; /* return value from recv() */

    /* Используется IPPROTO_UDP, т.к. будем принимать udp пакеты 
     * (IPPROTO_RAW не может быть использован для приема любых пакетов) */
    if ((raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket");
        return 1;
    }

    sockstr.sin_family = AF_INET;
    sockstr.sin_port = htons(PORT_TO_BIND);
    sockstr.sin_addr.s_addr = inet_addr(ADDR_TO_BIND);
    socklen = (socklen_t) sizeof(sockstr);

    if (bind(raw_socket, (struct sockaddr*) &sockstr, socklen) == -1) {
        perror("bind");
    }

    memset(msg, 0, MSG_SIZE);
    
    
	/*echo part*/
    while (1)
    {
        char input[100];
        fgets(input, 100, stdin);
        strcpy(data, input);
        
        
        strcpy(source_ip , "127.0.0.1");
     
		sin.sin_family = AF_INET;
		sin.sin_port = htons(80);
		sin.sin_addr.s_addr = inet_addr ("127.0.0.1");
     
		iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data);
     
		udph->len = htons(8 + strlen(data)); //tcp header size
		udph->check = 0; //leave checksum 0 now, filled later by pseudo header
     
		psh.source_address = inet_addr( source_ip );
		psh.dest_address = sin.sin_addr.s_addr;
		psh.placeholder = 0;
		psh.protocol = IPPROTO_UDP;
		psh.udp_length = htons(sizeof(struct udphdr) + strlen(data) );
     
		psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
		pseudogram = malloc(psize);
     
		memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
		memcpy(pseudogram + sizeof(struct pseudo_header) , udph , sizeof(struct udphdr) + strlen(data));
     
		udph->check = csum( (unsigned short*) pseudogram , psize);
        
        
        printf("to send:%s\nsize: %ld\n", data, strlen(data));
        
        
        //Send the packet
        if (sendto (s, datagram, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
        {
            perror("sendto failed");
        }
        //Data send successfully
        
        else
        {
            printf("%s\n", data);
            printf ("Packet Send. Length : %d \n" , iph->tot_len);
        }
       
        if ((msglen = recv(raw_socket, msg, MSG_SIZE, 0)) == -1) {
			perror("recv");
		}
        
        if ((msglen = recv(raw_socket, msg, MSG_SIZE, 0)) == -1) {
			perror("recv");
		
        }
		if (msglen <= HEADER_SIZE) /* msg  can't be lesser than header! */
			printf("No msg!\n");
		else {
			printf("msglen:%ld\nmsg: %s\n", msglen, msg + HEADER_SIZE);
		}
       
    }
     
    return 0;
}

