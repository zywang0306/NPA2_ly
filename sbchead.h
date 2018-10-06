// Author : Yu Lei
// Last Update: 
// Used for: define the datastruct for message
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/
//static const int PAYLOAD_LEN 512
#define PAYLOAD_LEN 512

//type of message
#define JOIN 2
#define SEND 4
#define FWD 3

//type of attribute
#define USERNAME 2
#define MESSAGE 4
#define REASON 1
#define CLIENT_COUNT 3


struct SBCP_Attribute{
    unsigned int Type : 16;
    unsigned int Length : 16;
    char Payload[512];
};

struct SBCP_Message{
    unsigned int Vrsn : 9;
    unsigned int Type : 7;
    unsigned int Length : 16;
    struct SBCP_Attribute attribute;
};