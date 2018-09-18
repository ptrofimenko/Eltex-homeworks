#ifndef SNIFF_LIB_H
#define SNIFF_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pcap.h>
#include <ctype.h>

typedef struct {
	char *dev_name;
	pcap_t* dev_id;
	bpf_u_int32 mask;
	bpf_u_int32 ip;
	const char *filter;
	struct pcap_pkthdr hdr;
	struct ether_header *eptr; 
	struct bpf_program fp;
} data_type;

void init (void *args);
void callback(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet);

#endif	
