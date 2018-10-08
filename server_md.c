// Author : Yu Lei
// Last Update: 

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
char user_name[16];
int user_number = 0;
int flag = 0;

void fwd_MSG(int socket_fd, struct SBCP_Message *message_from_client, struct SBCP_Message *message_to_client){
    struct SBCP_Attribute attribute;
    attribute.Type = USERNAME;
    attribute.Length = 4 + sizeof(user_name);
    bzero((char*)&attribute.Payload,sizeof(attribute.Payload));
    strcpy(attribute.Payload, user_name);
        
    message_to_client->Vrsn = 3;
    message_to_client->Type = FWD;
    message_to_client->Length = 8 + sizeof(user_name);
    message_to_client->attribute = attribute;
    if(write(socket_fd, message_to_client, sizeof(struct SBCP_Message)) < 0){
        printf("Failed...\n");
        perror("Error : Failed to FWD usename...\n");
        exit(0);
    }else{
//            printf("Send to the client successfully...\n");
//            printf("The length is %d...\n", sizeof(struct SBCP_Message));
    }
    char* messages = message_from_client->attribute.Payload; 
    message_to_client->Length = 8 + sizeof(messages);
    message_to_client->attribute.Type = MESSAGE;
    message_to_client->attribute.Length = 4 + sizeof(messages);
    bzero((char*)&attribute.Payload,sizeof(message_to_client->attribute.Payload));
    strcpy(message_to_client->attribute.Payload, messages);
    if(write(socket_fd, message_to_client, sizeof(struct SBCP_Message)) < 0){
        printf("Failed...\n");
        perror("Error : Failed to FWD message...\n");
        exit(0);
    }else{
        printf("Send to the client successfully...\n");
    }
}

void read_MSG(int socket_fd, struct SBCP_Message *message_from_client, struct SBCP_Message *message_to_client, ){
    int readno;
    readno = read(socket_fd, message_from_client, sizeof(struct SBCP_Message));
    if(readno < 0){
        printf("Failed...\n");
        perror("Error : Failed to read the FWD message from the server...\n");
        exit(0);
    }
    
    
    if(message_from_client->Vrsn != 3 || (message_from_client->Type != JOIN && message_from_client->Type != SEND)){
        return;
    } // can not read the header

    if((message_from_client->attribute.Type != USERNAME &&  message_from_client->attribute.Type != MESSAGE)){
        return;
    } // can not read the header of attribute
    
    printf("Try to read...\n");
    
    if(message_from_client->Type == JOIN && message_from_client->attribute.Type == USERNAME){
        strcpy(user_name, message_from_client->attribute.Payload);
        if(isExist(user_name)){
            continue;
        }
        printf("The joined user is %s.\n", user_name);
        user_number++;
        printf("The number of joined users is %d.\n", user_number);
    }
    
    if(message_from_client->Type == SEND && message_from_client->attribute.Type == MESSAGE){
        flag = 1;
        printf("received message %s", message_from_client->attribute.Payload);
        printf("User %s says: %s", user_name, message_from_client->attribute.Payload);
        int i;
        for(i = 0; i < user_number; i++){
            Flag_sent[i] = 1;
        }
    }    
    return;
}



int main(int argc, char* argv[]){
    struct sockaddr_in sin;
    int len;
    int socket_id, new_socket_id; //This is the socket
//we don't need buf in the main.
    char Payload[512];
    
    clients = (struct SBCP_Client *)malloc(maxClients*sizeof(struct SBCP_client_info));
    
    
    if(argc != 4){
        fprintf(stderr, "usage: simplex - talk host\n");
        exit(0);
    }
    
    char* server_address = argv[1];
    char* port_no = argv[2];
    int max_clients = atoi(argv[3]);
    int Flag_sent[512];
    printf("The port number from command line is %s\n",port_no);
    
//    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(server_address); // maysbe ip address;
    sin.sin_port = htons(atoi(port_no));
    printf("The max_clients is %d\n", max_clients);
    
    struct SBCP_Message *message_from_client;
    struct SBCP_Message *message_to_client;
    message_from_client = malloc(sizeof(struct SBCP_Message));
    message_to_client = malloc(sizeof(struct SBCP_Message));
//    bzero()
    /* setup passive open*/
    if((socket_id = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Simplex - talk : socket");
        exit(0);
    }
    printf("Socket successfully created!\n");
    
    
    if((bind(socket_id, (struct sockaddr *)&sin, sizeof(sin))) < 0){
        perror("simplex -talk: bind\n");
        exit(0);
    }else{
        printf("Socket binded.\n");
    }
    
    printf("Listening to the client...\n");
    
    if(listen(socket_id, MAX_PEDING)< 0){
        perror("Cannot find the client!\n");
    }
    
//    message_to_client = malloc(sizeof(struct SBCP_Message));
    fd_set readfds, fd_temp;
    int fdmax, i, j;
    FD_ZERO(&readfds);
    FD_SET(STDIN, &readfds);
    FD_SET(socket_id, &readfds);
//    FD_SET(socket_fd, &readfds);
    fdmax = socket_id + 1;
    pid_t ppid, pid;
    
    while(1){
        fd_temp = readfds;
//        printf("Begin to SELECT!\n");
        if(select(fdmax, &fd_temp,NULL ,NULL, NULL) < 0){
            perror("Error: select\n");
            close(socket_id);
            exit(0);
        }
//        printf("CAN SELECT!\n");
        for(i = 0; i < fdmax; i++){
            if(FD_ISSET(i, &fd_temp)){
                if(i == socket_id){
                    if((new_socket_id = accept(socket_id,(struct sockaddr *)&sin, &len)) <0){
                        perror("simplex - talk: accept\n");
                        exit(0);
                    }
                    printf("CAN ACCEPT! And new_socket_id is : %d\n", new_socket_id);
                    FD_SET(new_socket_id, &readfds);
                    if(new_socket_id > fdmax){
                        fdmax = new_socket_id + 1;
                    }
                }else{
                    read_MSG(i, message_from_client, message_to_client);
                    if(flag == 1){
                        for(j = 0; j < fdmax; j++){
                            if(FD_ISSET(j, &readfds) && j != socket_id){
                                fwd_MSG(j, message_from_client, message_to_client);
                            }
                        }
                        flag = 0;
                    }
                }
            }
        }
        
 /*       
        
        if((new_socket_id = accept(socket_id,(struct sockaddr *)&sin, &len)) <0){
            perror("simplex - talk: accept\n");
            exit(0);
        }
        int user_n = user_number + 1;
//        printf("Link a new user, sock_fd is : %d\n", new_socket_id);
        ppid = fork();
        
        if(ppid < 0){
            perror("Failure to fork 1");
            exit(0);
        }
        if (ppid == 0) { // The child process is generated.
            pid = fork();
            if(pid < 0){
                perror("Failure to fork 1");
            }
            if(pid == 0){
                while(1){
                    printf("this user_number is: %d\n", user_n);
                    read_MSG(new_socket_id, message_from_client, message_to_client, Flag_sent);
                }
            }
        }
        //    subprocess(new_socket_id); // Fork successfully and try to handle the messages
        //    exit(0);
        else{
             close(new_socket_id);
        }
        */
/*        
        while(1){
            read_MSG(new_socket_id, message_from_client, message_to_client);
        }
        close(new_socket_id);
        */
    }
    
    return 0;
}