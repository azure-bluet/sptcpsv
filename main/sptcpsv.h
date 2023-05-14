#ifndef SPTCPSV
#define SPTCPSV

#include <sys/socket.h>

/* -I/usr/bin/sptcpsv */
#include <exitv.h>
#include <logfile.h>
#include <queue.h>
#include <utils.h>

struct pairskt
{
    int sockfd;
    void* token;
};

typedef struct pairskt* (*GetToken) (int, struct sockaddr_in);
typedef void (*libinit) (struct Logger*);
typedef struct pairskt* (*ConnHandler) (struct pairskt*, int, char*);

/*
The framework uses the function init to initialize the lib.
It uses get_token when a new client connected.
It uses handle to handle a task.
*/

#endif
