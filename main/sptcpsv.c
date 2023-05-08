#include <assert.h>
#include <dlfcn.h>
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

#include "logfile.h"
#include "queue.h"
#include "utils.h"

#include "exitv.h"

#ifndef THREAD_POOL_SIZE
#define THREAD_POOL_SIZE 32
#endif

#ifndef LENQUEUE
#define LENQUEUE 131072
#endif

ConnHandler handler;

pthread_t thread_pool [THREAD_POOL_SIZE];
struct queue q;

void* thread (void* arg)
{
    int thrn = (int) arg;
    while (1)
    {
        struct pairskt* p = pop_q (q);
        if (p == NULL) continue;
        p = handler (p, thrn);
        if (p != NULL) while (push_q (q, p));
    }
}

int main (int argc, char** argv)
{
    struct sockaddr_in v4;
    unsigned short port;
    struct Logger logger;
    int sock, i;
    struct in_addr addr;
    void* lib;
    libinit init;
    GetToken tkg;
    char* tmu = strtmu ();
    log_init (&logger, tmu);
    free (tmu);
    memset (&v4, 0, sizeof (v4));
    switch (argc)
    {
        case 1:
            v4.sin_addr.s_addr = htonl (INADDR_ANY);
            v4.sin_port = htons (14444);
        case 2:
            sscanf (argv [1], "%hu", &port);
            v4.sin_addr.s_addr = htonl (INADDR_ANY);
            v4.sin_port = htons (port);
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
                    exit (INVALID_ARGUMENTS);
                }
                if (sscanf (argv [1], "%hu", &port) == 0) goto INV_PAR;
            }
            else if (sscanf (argv [1], "%hu", &port) == 0) goto INV_PAR;
            v4.sin_port = htons (port);
        default: goto INV_PAR;
    };
    lib = dlopen ("main.so", RTLD_NOW);
    sock = FAILED_OPENING_FILE;
    if (lib == NULL)
    {
        log_fatal (&logger, "MAIN", "Cannot load the server program!");
        exit (FAILED_OPENING_FILE);
    }
    init = (libinit) dlsym (lib, "init");
    tkg = (GetToken) dlsym (lib, "get_token");
    handler = (ConnHandler) dlsym (lib, "handle");
    if (init == NULL || tkg == NULL || handler == NULL)
    {
        log_fatal (&logger, "MAIN", "Cannot load the server program!");
        exit (INVALID_INPUT);
    }
    init (&logger);
    pthread_mutex_init (&(q.lock), NULL);
    q.len = LENQUEUE;
    q.objs = calloc (LENQUEUE, sizeof (void*));
    if (q.objs == NULL)
    {
        log_fatal (&logger, "MAIN", "Memory not enough!");
    }
    for (i=0; i<THREAD_POOL_SIZE; i++)
    {
        pthread_t tid;
        if (pthread_create (&tid, NULL, thread, (void*) i) || pthread_detach (tid))
        {
            log_fatal (&logger, "MAIN", "Failed to create thread!");
            exit (FAILED_CREATING_THREAD);
        }
        thread_pool [i] = tid;
    }
    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        log_fatal (&logger, "MAIN", "Cannot create socket!");
        exit (FAILED_SOCKET);
    }
    v4.sin_family = AF_INET;
    if (bind (sock, (struct sockaddr*) &v4, sizeof (struct sockaddr_in)) == -1)
    {
        log_fatal (&logger, "MAIN", "Cannot bind socket!");
        exit (FAILED_BIND);
    }
    if (listen (sock, 100) == -1)
    {
        log_fatal (&logger, "MAIN", "Cannot listen on the port!");
        exit (FAILED_LISTEN);
    }
    while (1)
    {
        struct sockaddr caddr;
        int cls = accept (sock, &caddr, NULL);
        if (cls == -1)
        {
            log_error (&logger, "MAIN", "Failed to accept a connection!");
            continue;
        }
        struct pairskt* skt = tkg (cls, caddr);
        push_q (q, (void*) skt);
    }
    return 0;
}
