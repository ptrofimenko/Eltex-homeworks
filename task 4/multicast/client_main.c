#include "lib.h"
#include "lib.c"

int main() {
	printf("***CLIENT***\n");
	
	data_type data;
	bzero((char *)&data.serveraddr, sizeof(data.serveraddr));
	data.protocol = IPPROTO_UDP;
	data.addr = htonl(INADDR_ANY);
	data.addrlen = sizeof(data.serveraddr);
	
	init((void *)&data);
	
	struct ip_mreq mreq;
	if (bind(data.sock, (struct sockaddr *) &data.serveraddr, sizeof(data.serveraddr)) < 0) {        
        perror("bind");
		exit(1);
    }   
    
    mreq.imr_multiaddr.s_addr = inet_addr(GROUP);         
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
    if (setsockopt(data.sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		perror("setsockopt mreq");
		exit(1);
    }
    data.addrlen = sizeof(data.serveraddr);
    int size = 0;         
    while (1) {
		size = recvfrom(data.sock, data.message, sizeof(data.message), 0, (struct sockaddr *) &data.serveraddr, &data.addrlen);
		if (size < 0) {
			perror("recvfrom");
			exit(1);
		}
		printf("%s: message = \"%s\"\n", inet_ntoa(data.serveraddr.sin_addr), data.message);
    }
}
