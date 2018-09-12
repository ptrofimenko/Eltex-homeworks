#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>


#define BYE_OFFICE 12346
#define HELLO_PORT 12345
#define HELLO_GROUP "255.255.255.255"

int main()
{

    struct sockaddr_in addr;
    struct sockaddr_in addr2;
    int fd;
    int fd2;
    char *message = "1";
    char *message2 = "2";

    if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("socket");
        exit(1);
    }

    if ((fd2 = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("socket");
        exit(1);
    }
    
    /* set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_addr.s_addr = inet_addr(HELLO_GROUP);
    addr.sin_port=htons(HELLO_PORT);

    memset(&addr2,0,sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_addr.s_addr = htonl(INADDR_ANY);
    addr2.sin_addr.s_addr = inet_addr(HELLO_GROUP);
    addr2.sin_port=htons(BYE_OFFICE);
    
    int enable = 1;
	setsockopt(fd,SOL_SOCKET,SO_BROADCAST,&enable,sizeof(enable));
	setsockopt(fd2,SOL_SOCKET,SO_BROADCAST,&enable,sizeof(enable));
	
	 if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		 perror("bind");
	}
        
     if (bind(fd2, (struct sockaddr *) &addr2, sizeof(addr2)) < 0) {
        perror("bind");
     }
     
   
	unsigned int addr_size = sizeof(addr);
	unsigned int addr2_size = sizeof(addr2); 
    while (1)
    {
		
        if (recvfrom(fd, message, 100, 0,NULL, 0) < 0)
        //if (recvfrom(fd, message, 100, 0,(struct sockaddr *) &addr, &addr_size) < 0)
        {
            perror("recvfrom");
            //exit(1);
        }
        printf("%s\n", message);
        /*if (recvfrom(fd2, message2, 100, 0,NULL, 0) < 0)
        //if (recvfrom(fd2, message2, 100, 0,(struct sockaddr *) &addr2, &addr2_size) < 0)
        {
            perror("recvfrom2");
            //exit(1);
        }*/
        //printf("%s\n", message2);
    }
}
