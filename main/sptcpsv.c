#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "sptcpsv.h"

#ifndef THREAD_POOL_SIZE
#define THREAD_POOL_SIZE 32
#endif

#ifndef LENQUEUE
#define LENQUEUE 131072
#endif

ConnHandler handler;

pthread_t thread_pool [THREAD_POOL_SIZE];
queue* q;

struct Logger logger;

void* thread (void* arg)
{
    int thrn = (int) arg;
    char th [32];
    sprintf (th, "THREAD/%d", thrn);
    log_info (&logger, th, "Thread started");
    while (1)
    {
        struct pairskt* p = pop_q (q);
        if (p == NULL) continue;
        log_info (&logger, th, "Handling a new task in the queue");
        p = handler (p, thrn, th);
        if (p != NULL) while (push_q (q, p));
        else continue;
        log_info (&logger, th, "Pushed a new task to the queue");
    }
}

int main (int argc, char** argv)
{
    struct sockaddr_in v4;
    unsigned short port;
    int sock, i;
    struct in_addr addr;
    void* lib;
    libinit init;
    GetToken tkg;
    int skl = sizeof (struct sockaddr_in);
    char* tmu = strtmu ();
    log_init (&logger, tmu);
    free (tmu);
    memset (&v4, 0, sizeof (v4));
    switch (argc)
    {
        case 1:
            v4.sin_addr.s_addr = htonl (INADDR_ANY);
            v4.sin_port = htons (14444);
            break;
        case 2:
            sscanf (argv [1], "%hu", &port);
            v4.sin_addr.s_addr = htonl (INADDR_ANY);
            v4.sin_port = htons (port);
            break;
        case 3:
            tmu = malloc (80);
            sscanf (argv [1], "%s", tmu);
            if (inet_aton (tmu, &(v4.sin_addr)) == 0)
            {
                sscanf (argv [2], "%s", tmu);
                if (inet_aton (tmu, &(v4.sin_addr)) == 0)
                {
                    INV_PAR:
                    log_fatal (&logger, "MAIN", "Something wrong happened while"
                        " parsing arguments!");
                    return INVALID_ARGUMENTS;
                }
                if (sscanf (argv [1], "%hu", &port) == 0) goto INV_PAR;
            }
            else if (sscanf (argv [1], "%hu", &port) == 0) goto INV_PAR;
            v4.sin_port = htons (port);
            break;
        default: goto INV_PAR;
    };
    lib = dlopen ("./main.so", RTLD_NOW);
    if (lib == NULL)
    {
        log_fatal (&logger, "MAIN", "Cannot load the server program!");
        return FAILED_OPENING_FILE;
    }
    init = (libinit) dlsym (lib, "init");
    tkg = (GetToken) dlsym (lib, "get_token");
    handler = (ConnHandler) dlsym (lib, "handle");
    if (init == NULL || tkg == NULL || handler == NULL)
    {
        log_fatal (&logger, "MAIN", "Illegal server program!");
        return INVALID_INPUT;
    }
    init (&logger);
    q = new_q (LENQUEUE);
    for (i=0; i<THREAD_POOL_SIZE; i++)
    {
        pthread_t tid;
        if (pthread_create (&tid, NULL, thread, (void*) i))
        {
            log_fatal (&logger, "MAIN", "Failed to create thread!");
            return FAILED_CREATING_THREAD;
        }
        thread_pool [i] = tid;
    }
    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        log_fatal (&logger, "MAIN", "Cannot create socket!");
        return FAILED_SOCKET;
    }
    v4.sin_family = AF_INET;
    if (bind (sock, (struct sockaddr*) &v4, sizeof (struct sockaddr_in)) == -1)
    {
        log_fatal (&logger, "MAIN", "Cannot bind socket!");
        return FAILED_BIND;
    }
    if (listen (sock, 100) == -1)
    {
        log_fatal (&logger, "MAIN", "Cannot listen on the port!");
        return FAILED_LISTEN;
    }
    while (1)
    {
        struct sockaddr_in caddr;
        char er [150];
        int cls = accept (sock, (struct sockaddr*) &caddr, (socklen_t*) &skl);
        if (cls == -1)
        {
            sprintf (er, "ERROR: %s", strerror (errno));
            log_error (&logger, "MAIN", "Failed to accept a connection!");
            log_error (&logger, "MAIN", er);
            continue;
        }
        else
        {
            char inf [400];
            sprintf (inf, "%s:%d connected!", inet_ntoa (caddr.sin_addr), ntohs
                (caddr.sin_port));
            log_info (&logger, "MAIN", inf);
        }
        struct pairskt* skt = tkg (cls, caddr);
        push_q (q, (void*) skt);
        log_info (&logger, "MAIN", "Pushed a new task to the queue");
    }
    return 0;
}
