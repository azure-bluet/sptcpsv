#ifndef SPTCPSV
#define SPTCPSV

/* -I/usr/bin/sptcpsv */
#include <exitv.h>
#include <logfile.h>
#include <queue.h>
#include <utils.h>

#ifdef __cplusplus

namespace sptcpsv
{

#endif

struct pairskt
{
    int sockfd;
    void* token;
};

#ifdef __cplusplus

extern "C"
{

#endif

typedef int* (*libinit) (struct Logger*);
typedef struct pairskt* (*GetToken) (int, struct sockaddr_in, unsigned short);
typedef struct pairskt* (*ConnHandler) (struct pairskt*, int, char*);
typedef void (*serverclose) (void);

/*
The framework uses the function init to initialize the lib.
It uses get_token when a new client connected.
It uses handle to handle a task.
*/


#ifdef __cplusplus

};
};

#endif

#endif
