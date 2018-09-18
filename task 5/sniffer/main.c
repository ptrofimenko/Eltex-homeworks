#include "sniff_lib.h"
#include "sniff_lib.c"

int main() {
	data_type data;
	data.filter = "udp or tcp or icmp or ip";
	init((void *)&data);
	pcap_loop(data.dev_id, -1, callback, NULL); 
	return 0;
}
