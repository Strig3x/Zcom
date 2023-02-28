#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "flags.c"

#define IP__ 0x00
#define TCP__ 0x06
#define UDP__ 0x11

struct socket_id{
	int socket_fd;			//Socket file descriptor.
	int addr_domain;	//Address family(i.e. IPv4, IPv6).
	unsigned long id_int;	//Integer ID representation(i.e. 111111).
    unsigned int protocol_type;	//Protocol type(i.e. TCP, UDP).
};

int log_socket_activities(struct socket_id* sfd_info){		//Log socket activities temporarily.
	FILE* sfd_info_file;
	const char* sfd_info_file_name = "fdlist.txt";
	char log_string[350];
	
	if((sfd_info_file = fopen(sfd_info_file_name, "a"))==NULL){		//Check for file before appending data.
		printf(">Log file does not exist, creating %s.\n", sfd_info_file_name);
		if((sfd_info_file = fopen(sfd_info_file_name, "w"))==NULL){		//Create log file.
			printf("!>Failed to create %s.\n", sfd_info_file_name);
			return -1;
		}else{
			if(fclose(sfd_info_file)==EOF){
				printf("!>Failed recursion.\n");
				return -1;
			}
			log_socket_activities(sfd_info);		//Recursion.
		}
	}
//Create socket info string:
	if(sprintf(log_string, "\nID: [%lu]\n", sfd_info->id_int)<0){
		printf("!>Failed to format log string.[1]\n");
		return -1;
	}

	if(sfd_info->addr_domain==PF_INET){
		if(strcat(log_string, "INET Type: IPv4\n")==NULL){
			printf("!>Failed to format log string.[2]\n");
			return -1;
		}
	}else if(sfd_info->addr_domain==AF_INET6){
		if(strcat(log_string, "INET Type: IPv6\n")==NULL){
			printf("!>Failed to format log string.[2]\n");
			return -1;
		}
	}

	if(sfd_info->protocol_type==TCP__){
		if(strcat(log_string, "Protocol: TCP\n")==NULL){
			printf("!>Failed to format log string.[3]\n");
			return -1;
		}
	}else if(sfd_info->protocol_type==UDP__){
		if(strcat(log_string, "Protocol: UDP\n")==NULL){
			printf("!>Failed to format log string.[3]\n");
			return -1;
		}
	}

	if(fprintf(sfd_info_file, "%s", log_string)<0){
		printf("!>Failed to document socket data.\n");
		return -1;
	}

	if(fclose(sfd_info_file)==EOF){
		printf("!>Failed to close %s.\n", sfd_info_file_name);
		return -1;
	}
	
	return 0;
}

GLOBAL_F_ FLAG_previous_id;
void generate_sfd_id(struct socket_id* sfd_info){	//Generate random ID.
	srand(time(NULL));		//call time to randomize random generator seed.
	long val_max = 999999;	//Set maximum value.
    unsigned long id__;
regenerate:
	id__ = rand() % 900000 + 100000;

	if(id__>=val_max||id__<=0){
		printf("!>Generated invalid ID, retrying...\n");
		goto regenerate;
	}
	
	printf(">[debug] ID selected: %ld.\n", id__);

	sfd_info->id_int = id__; puts(">[debug] Set integer ID.\n");	//Set integer ID.

	return;
}

int generate_basic_socket(unsigned int type, int addr_fam){
	int sfd;
	struct socket_id* current_sfd = (struct socket_id*)malloc(sizeof(struct socket_id));	puts(">[debug] Created socket structure.\n");

	current_sfd->addr_domain = addr_fam;	puts(">[debug] Set address family.\n");
	current_sfd->protocol_type = type;		puts(">[debug] Set protocol type.\n");
	puts(">[debug] Creating ID...\n");
	generate_sfd_id(current_sfd);		//Set ID of the current socket file descriptor.
	
	if(current_sfd->id_int==FLAG_previous_id){
		printf("!>Generated identical ID, regenerating...\n");
		generate_sfd_id(current_sfd);
	}

	if(type==TCP__){
		sfd = socket(addr_fam, SOCK_STREAM, TCP__);
		if(sfd<0){
			printf("!>Failed to create TCP socket.\n");
			return -1;
		}
	}else if(type==UDP__){
		sfd = socket(addr_fam, SOCK_DGRAM, UDP__);
		if(sfd<0){
			printf("!>Failed to create UDP socket.\n");
			return -1;
		}
	}
	current_sfd->socket_fd = sfd;

	int log_status = log_socket_activities(current_sfd);
	if(log_status!=0){
		printf("!>Failed to log socket [%lu].\n", current_sfd->id_int);
	}

	FLAG_previous_id = current_sfd->id_int;	

	free(current_sfd);
	return 0;
}
//<<<<<<<<<<<< Fix failure to write to file.
//Fixed it!
