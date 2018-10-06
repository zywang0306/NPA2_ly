// Author : Yu Lei
// Last Update: 
// Used for: define the datastruct for message

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int MAX_PEDING = 512;
static const int PAYLOAD_LEN = 512;
//#define PAYLOAD_LEN 512

//type of message
static const int JOIN = 2;
static const int SEND = 4;
static const int FWD = 3;

//type of attribute
static const int USERNAME = 2;
static const int MESSAGE = 4;
static const int REASON = 1;
static const int CLIENT_COUNT = 3;


struct SBCP_Attribute{
    unsigned int Type;
    unsigned int Length;
    char Payload[512];
};

struct SBCP_Message{
    unsigned int Vrsn;
    unsigned int Type;
    unsigned int Length;
    struct SBCP_Attribute attribute;
};
