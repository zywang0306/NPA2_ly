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
int Max_Client;
struct SBCP_Client *client;
struct sockaddr_in6 my_addr, their_addr; // IPv6

// IPv6
void ipv6connect(int myport, int lisnum, struct sockaddr_in6 my_addr ) {
	socklen_t len;
	int sockfd, new_fd;
	char buf[1024];
	struct sockaddr_in6 their_addr;


	if ((sockfd = socket(PF_INET6, SOCK_STREAM, 0)) == -1) { // IPv6
		perror("socket");
		exit(1);
	}
	else
		printf("socket created/n");

	bzero(&my_addr, sizeof(my_addr));

	my_addr.sin6_family = PF_INET6;    // IPv6
	my_addr.sin6_port = htons(myport);   // IPv6
		my_addr.sin6_addr = in6addr_any;            // IPv6

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in6))  // IPv6
		== -1) {
		perror("bind");
		exit(1);
	}
	else
		printf("binded/n");

	if (listen(sockfd, lisnum) == -1) {
		perror("listen");
		exit(1);
	}
	else
		printf("begin listen/n");

	while (1) {
		len = sizeof(struct sockaddr);
		if ((new_fd =
			accept(sockfd, (struct sockaddr *) &their_addr,
				&len)) == -1) {
			perror("accept");
			exit(errno);
		}
		else
			printf("server: got connection from %s, port %d, socket %d/n",
				inet_ntop(AF_INET6, &their_addr.sin6_addr, buf, sizeof(buf)), // IPv6
				their_addr.sin6_port, new_fd); // IPv6
	}
}
void join_user(int sock_fd, char* user_name_, struct SBCP_Client *client){
    int curr_index = user_number - 1;
    client[curr_index].sock_fd = sock_fd;
    client[curr_index].online_flag = 1;
    strcpy(client[curr_index].username, user_name_);
//    printf("Successfully JOIN the user %s\n", client[curr_index].username);
//    client[curr_index]->online_flag = 0;
}

/*
void delete_user(int sock_fd, char* user_name, struct SBCP_Client *client){
    int i;
    for(i = 0; i < user_number; i++){
        if(client[i] ->sock_fd == sock_fd){
            if(i != (user_number - 1)){
                client[i]->sock_fd = client[user_number - 1]->sock_fd;
                strcpy(client[i]->user_name,client[user_number - 1]->user_name);
                client[user_number - 1] -> online_flag = 0;
                user_number--;
                return;
            }else{
                client[user_number - 1] -> online_flag = 0;
                user_number--;
                return;
            }
        }
    }
}
*/
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
        printf("FWD to the client successfully...\n");
    }
}

void read_MSG(int socket_fd, struct SBCP_Message *message_from_client, struct SBCP_Message *message_to_client, struct SBCP_Client *client){
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
        {
        //if(!isExist(user_name)){
            printf("The joined user is %s.\n", user_name);
            if(user_number < Max_Client){
                user_number++;
                printf("The number of joined users is %d.\n", user_number);
                join_user(socket_fd, user_name, client);
            }else{
                printf("Warning: The number of connected clients is reached upper limit! Cannot JOIN!!!\n");
            }
            
        }
    }
    
    if(message_from_client->Type == SEND && message_from_client->attribute.Type == MESSAGE){
        flag = 1;
        printf("received message %s", message_from_client->attribute.Payload);
        printf("User %s says: %s", user_name, message_from_client->attribute.Payload);
/*        int i;
        for(i = 0; i < user_number; i++){
            Flag_sent[i] = 1;
        }
        */
    }    
    return;
}



int main(int argc, char* argv[]){
    struct sockaddr_in sin;
    int len;
    int socket_id, new_socket_id; //This is the socket
//we don't need buf in the main.
    char Payload[512];
    
    
    
    if(argc != 4){
        fprintf(stderr, "usage: simplex - talk host\n");
        exit(0);
    }
    
    char* server_address = argv[1];
    char* port_no = argv[2];
    Max_Client = atoi(argv[3]);
    client = (struct SBCP_Client *)malloc(Max_Client*sizeof(struct SBCP_Client));
    int Flag_sent[512];
    printf("The port number from command line is %s\n",port_no);
    
//    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(server_address); // maysbe ip address;
    sin.sin_port = htons(atoi(port_no));
    printf("The max_clients is %d\n", Max_Client);
    
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
    int fdmax, i, j, k;
    FD_ZERO(&readfds);
//    FD_SET(STDIN, &readfds);
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
                printf("This is select i : %d\n", i);
                if(i == socket_id){
                    if((new_socket_id = accept(socket_id,(struct sockaddr *)&sin, &len)) <0){
                        perror("simplex - talk: accept\n");
                        exit(0);
                    }
                    printf("CAN ACCEPT! And new_socket_id is : %d\n", new_socket_id);
                    //read_MSG(new_socket_id, message_from_client, message_to_client, client);
                    FD_SET(new_socket_id, &readfds);
                    FD_SET(new_socket_id, &fd_temp);
                    if(new_socket_id > (fdmax - 1)){
                        fdmax = new_socket_id + 1;
                    }
                }else{
                    read_MSG(i, message_from_client, message_to_client, client);
                    if(flag == 1){
                        printf("Now the user list:\n");
                        for(k = 0; k < user_number; k++){
                            printf("The %dst client is %s.\n", k, client[k].username);
                        }
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
