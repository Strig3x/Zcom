#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "flags.c"
#include "internal_operations.c"
#include "external_operations.c"
#include "communications.c"

int console_args(char* inp){
    if(strstr(inp, "list")){
        read_file();
        return 0;
    }
    if(strstr(inp, "locate")){
        test_connections();
        return 0;
    }
/*   extra args   */
    if(strstr(inp, "cs")){
        system("clear");
        return 0;
    }
    if(strstr(inp, "zcom")){
        system("cat zcom.txt");
        return 0;
    }
	if(strstr(inp, "sgen")){
		if(generate_basic_socket(6, PF_INET)!=0){
			printf("!>Failed to generate socket.\n");
		}
		return 0;
	}
    if(strstr(inp, "exit")){
        puts("\n>Exiting...\n");
        exit(0);
    }
    
    return 1;
}
