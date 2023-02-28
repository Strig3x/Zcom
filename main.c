#include <stdio.h>
#include <stdlib.h>
#include "arg_cmds.c"

void console(void){
    char input_buf[256];
    
    while(1){
        printf("\n[CONSOLE]: ");
        fgets(input_buf, 256, stdin);
        if(console_args(input_buf)==1){
            printf("\n\t>>%s", input_buf);
        }
    }
    printf(">Closed console.\n");

    return;
}

int main(void){
    system("cat zcom.txt");
    console();
    exit(0);  
}

//Priorities:		STATUS: (2)
//~
//1. Setup locator server(clients should be autoconnecting to the central server).
//2. Setup communications method(custom datagrams and spoofing techniques).
//3. Work on communications(The needed functions should allow socket operations, datagram generation, packet/traffic analyzation, and logging abilities).
//
//Current Task:
//
//Working on communications.c . (fix segmentation fault).
