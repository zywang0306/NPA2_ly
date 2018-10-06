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

#include SbcHead.h


