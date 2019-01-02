
#include "tufilter.h"

int main(int argc, char *argv[]) {
	int i = 0, check = 0;
	filter_struct fil;
	
	
	for (i = 0; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");
	if (argc != 2 && argc != 6 && argc != 8) {
		wrong_format();
		return 0;
	}
	
	fil.type = 0;
	if (type(argc, argv, &fil)) {
		return 0;
	}
	
	if (fil.type == SET) {
		check = ip(argc, argv, &fil);
		if (check == 2) { //wrong ip
			return 0;
		} 
		check += port(argc, argv, &fil);
		if (check == 2) {
			printf("Port and ip doesn't set (need at least one)\n");
			wrong_format();
			return 0;
		}
		
		if(disable_enable(argc, argv, &fil) == 1) {
			printf("Option disable or enable required\n");
			wrong_format();
		}
	}
	printf("Finished successfully\n");
	return 0;
}

void wrong_format() {
	printf("Wrong arguments!\n");
	printf("Formant of input should be like \n./tufiler (--transport or --show) ");
	printf("--ip <ip> --port <port> --filter (enable or disable)\n");
	printf("If 1st arg is --show - no more args required\n");
}


int type(int argc, char *argv[], filter_struct *fil) {
	
	int i;
	
		
	for (i = 0; i < argc; i++) {
		
		if((strcmp("--transport", argv[i]) == 0) && i < (argc - 1)) {
		
			fil->type = SET;
			
			if (strcmp(argv[i + 1], "udp") == 0) {
				fil->transport = UDP;
				break;
			}
			else if (strcmp(argv[i + 1], "tcp") == 0) {
				fil->transport = TCP;
				break;
			}
			else {
				printf ("Bad transport protocol (udp or tcp required)\n");
				return 1;
			}
		}
		else if (strcmp("--show", argv[i]) == 0) {
			fil->type = SHOW;
			fil->transport = 0;
			fil->ip = NULL;
			fil->port = 0;
			fil->disable_enable = 0;
			break;
		}
	}
	if (fil->type == 0) {
		wrong_format();
		return 1;
	}
	return 0;
}

int ip(int argc, char *argv[], filter_struct *fil) { 
	
	int i;
	fil->ip = NULL;
	
	for(i = 0; i < argc; i++) {
		if((strcmp("--ip", argv[i]) == 0) && i < (argc - 1)) {
			if  (strlen(argv[i + 1]) > 15 || strlen(argv[i + 1]) < 7) {
				printf("Wrong ip lenght..exiting\n");
				return 2;
			}
			fil->ip = (char *) malloc(60 * sizeof(char));
			strcpy(fil->ip, argv[i + 1]);
			break;
		}
	}
	if(fil->ip == NULL) {
		return 1;
	}
	return 0;
}
	
int port(int argc, char *argv[], filter_struct *fil) {
	
	int i;
	fil->port = 0;
	
	for(i = 0; i < argc; i++) {
		if((strcmp("--port", argv[i]) == 0) && i < (argc - 1)) {
			fil->port = atoi(argv[i + 1]);
		}
	}
	if (fil->port == 0) { return 1; }
	return 0;
}

int disable_enable(int argc, char *argv[], filter_struct *fil) {
	
	int i;
	fil->disable_enable = 0;
	
	for(i = 0; i < argc; i++) {
		if(strcmp("disable", argv[i]) == 0) {
			fil->disable_enable = DIS;
		}
		else if (strcmp("enable", argv[i]) == 0) {
			fil->disable_enable = EN;
		}
	}
	if (fil->disable_enable == 0) {
		return 1;
	}
	return 0;
}
