// Author : Yu Lei
// Last Update: 
// Used for: define the datastruct for message
// Format: ./client username server_ip server_port
// initiate JOIN, can SEND and FWD messages
// use select to handle both sending and receving

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void join(){
    
}

void send_Message(){
    
}

int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr, "usage: simplex - talk host\n");
        exit(1);
    }
    
    char* username = argv[1];
    char* server_address = argv[2];
    char* port_no = argv[3];
    printf("The username from command line is %s\n",username);
    printf("The ip address from command line is %s\n",server_address);
    printf("The port number from command line is %s\n",port_no);
    
    struct sockaddr_in sin;
    char *host;
    int socket_fd;
    
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(server_address);
    sin.sin_port = htons(atoi(port_no));

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0))< 0){
        perror("simplex - talk : socket\n");
        exit(0);
    }else{
        printf("Socket successfully created!\n");
    }

}


