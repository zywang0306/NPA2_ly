// Author : Yu Lei
// Last Update: 
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

#include "sbchead.h"
char user_fwd_name[16];

/*
typedef struct{
    unsigned int Type : 16;
    unsigned int Length : 16;
    char Payload[512];
}SBCP_Attribute;

typedef struct{
    unsigned int Vrsn : 9;
    unsigned int Type : 7;
    unsigned int Length : 16;
    struct SBCP_Attribute attribute;
}SBCP_Message;
*/


//Join function try to send message to the server.
void join(char* username, int socket_fd, struct SBCP_Message *message_to_server){

    int user_len = strlen(username);
//    printf("The username is %s\n", username);
//    printf("The length of username is %d\n", user_len);
//    printf("The socket_fd is  %d\n", socket_fd);
    
    
    struct SBCP_Attribute attribute;
    attribute.Type = USERNAME;
    attribute.Length = 4 + user_len;
    bzero((char*)&attribute.Payload,sizeof(attribute.Payload));
    strcpy(attribute.Payload,username);
    
    message_to_server->Vrsn = 3;
    message_to_server->Type = JOIN;
    message_to_server->Length = 8 + user_len;
    message_to_server->attribute = attribute;
    
    printf("Ready to JOIN...\n");
//   printf("Len is %d\n", message_to_server->Length);
//    printf("Len is %d\n", message_to_server->Length);

//    if(write(socket_fd, message_to_server->attribute.Length, 2) < 0){
//    if(write(socket_fd, &(message_to_server->attribute.Payload), user_len) < 0){
    if(write(socket_fd, message_to_server, sizeof(struct SBCP_Message)) < 0){
        printf("Failed...\n");
        perror("Error : Failed to join to the server...\n");
        exit(0);
    }else{
        printf("Join to the server successfully...\n\n");
//        printf("The length is %d...\n", sizeof(struct SBCP_Message));
    }
    
    return;
}


void send_MSG(int socket_fd, struct SBCP_Message *message_to_server){
    char buf[PAYLOAD_LEN];
    fgets(buf, sizeof(buf) - 1, stdin);
    int lastIndex = strlen(buf) - 1;
    if(buf[lastIndex] == '\n'){
        buf[lastIndex] == '\0';
    }
    struct SBCP_Attribute attribute;
    attribute.Type = MESSAGE;
    attribute.Length = 4 + strlen(buf);
    bzero((char*)&attribute.Payload,sizeof(attribute.Payload));
    strcpy(attribute.Payload,buf);
    
    message_to_server->Vrsn = 3;
    message_to_server->Type = SEND;
    message_to_server->Length = 8 + strlen(buf);
    message_to_server->attribute = attribute;
    
    printf("Ready to SEND...\n");
    
    if(write(socket_fd, message_to_server, sizeof(struct SBCP_Message)) < 0){
        printf("Failed...\n");
        perror("Error : Failed to SEND to the server...\n");
        exit(0);
    }else{
        printf("SEND message to the server successfully...\n");
        printf("The length is %d...\n", sizeof(struct SBCP_Message));
    }
    return;
}

void read_MSG(int socket_fd, struct SBCP_Message *message_from_server){
    int readno;
    readno = read(socket_fd, message_from_server, sizeof(struct SBCP_Message));
    if(readno < 0){
        printf("Failed...\n");
        perror("Error : Failed to read the FWD message from the server...\n");
        exit(0);
    }
    
    if(message_from_server->Vrsn != 3 || message_from_server->Type != FWD || message_from_server->Length <= 0){
        return;
    } // can not read the header

    if((message_from_server->attribute.Type != 2 &&  message_from_server->attribute.Type != 4) || message_from_server->attribute.Length <= 0){
        return;
    } // can not read the header of attribute
    
//    printf("Try to read...\n");
    
    if(message_from_server->attribute.Type == 2){
        strcpy(user_fwd_name, message_from_server->attribute.Payload);
        printf("The user is %s.\n", user_fwd_name);
    }

    if(message_from_server->attribute.Type == 4){
        printf("User %s says: %s\n", user_fwd_name, message_from_server->attribute.Payload);
    }    
    return;
}

int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr, "Error: command is not correct...\n");
        exit(1);
    }
    
    char* username = argv[1];
    char* server_address = argv[2];
    char* port_no = argv[3];
    printf("The username from command line is %s\n", username);
    printf("The ip address from command line is %s\n", server_address);
    printf("The port number from command line is %s\n\n", port_no);
    
    struct sockaddr_in sin;
    char *host;
    int socket_fd;
    struct SBCP_Message *message_to_server;
    struct SBCP_Message *message_from_server;
    message_to_server = malloc(sizeof(struct SBCP_Message));
    message_from_server = malloc(sizeof(struct SBCP_Message));
    
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(server_address);
    sin.sin_port = htons(atoi(port_no));

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0))< 0){
        perror("Error : socket\n");
        exit(0);
    }else{
        printf("Socket successfully created...\n");
    }
    
//    int user_len = sizeof(username);

    if(connect(socket_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
        perror("Error: connect\n");
        close(socket_fd);
        exit(0);
    }else{
        printf("Socket connect successfully...\n", username);
    }

//    send_MSG(socket_fd, message_to_server);
    
    join(username, socket_fd, message_to_server); // Use JOIN to send the username.
    
//    printf("The message username is %s\n", message_to_server->attribute.Payload);
    
    //initialize select;
    fd_set readfds;
    int fdmax;
    FD_ZERO(&readfds);
    FD_SET(STDIN, &readfds);
    FD_SET(socket_fd, &readfds);
    fdmax = socket_fd + 1;
    //End initialize. 
    
    while(1){
//        printf("Enter while loop for select again...\n");
        if(select(fdmax, &readfds,NULL ,NULL, NULL) < 0){
            perror("Error: connect\n");
            close(socket_fd);
            exit(0);
        }
        
//        printf("Select successfully...\n");
        
        if(FD_ISSET(STDIN, &readfds)){
            send_MSG(socket_fd, message_to_server);
        }
        
        if(FD_ISSET(socket_fd, &readfds)){
            read_MSG(socket_fd, message_from_server);
        }
    }
    
    close(socket_fd);
}


