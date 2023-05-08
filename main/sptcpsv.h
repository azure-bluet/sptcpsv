#ifndef SPTCPSV
#define SPTCPSV

#include <sys/socket.h>

/* -I/usr/bin/sptcpsv */
#include "logfile.h"

struct pairskt
{
    int sockfd;
    void* token;
};

typedef struct pairskt* (*GetToken) (int, struct sockaddr);
typedef void (*libinit) (struct Logger*);
typedef struct pairskt* (*ConnHandler) (struct pairskt*, int);

/*
The framework uses the function init to initialize the lib.
It uses get_token when a new client connected.
It uses handle to handle a task.
*/

#endif
