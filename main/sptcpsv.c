#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
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

#ifndef MAX_LISTENERS
#define MAX_LISTENERS 32768
#endif

ConnHandler handler;

thrd_t thread_pool [THREAD_POOL_SIZE];
thrd_t listeners [MAX_LISTENERS];
queue* q;

struct Logger logger;
struct in_addr in;

GetToken tkg;

int listener (void* arg)
{
    int sock, thm, skl;
    unsigned short port;
    struct sockaddr_in v4;
    char thrn [140], prtn [100];
    memset (&v4, 0, sizeof (v4));
    port = * (unsigned short*) arg;
    v4.sin_addr = in;
    v4.sin_port = htons (port);
    thm = * (int*) (arg + sizeof (unsigned short));
    free (arg);
    sprintf (thrn, "LISTENER/%d", thm);
    sprintf (prtn, "Listener started at port %hu", port);
    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        log_fatal (&logger, thrn, "Cannot create socket!");
        return FAILED_SOCKET;
    }
    v4.sin_family = AF_INET;
    if (bind (sock, (struct sockaddr*) &v4, sizeof (struct sockaddr_in)) == -1)
    {
        log_fatal (&logger, thrn, "Cannot bind socket!");
        return FAILED_BIND;
    }
    if (listen (sock, 100) == -1)
    {
        log_fatal (&logger, thrn, "Cannot listen on the port!");
        return FAILED_LISTEN;
    }
    log_info (&logger, thrn, prtn);
    while (1)
    {
        struct sockaddr_in caddr;
        char er [150];
        int cls = accept (sock, (struct sockaddr*) &caddr, (socklen_t*) &skl);
        if (cls == -1)
        {
            sprintf (er, "ERROR: %s", strerror (errno));
            log_error (&logger, thrn, "Failed to accept a connection!");
            log_error (&logger, thrn, er);
            continue;
        }
        else
        {
            char inf [400];
            sprintf (inf, "%s:%d connected!", inet_ntoa (caddr.sin_addr), ntohs
                (caddr.sin_port));
            log_info (&logger, thrn, inf);
        }
        struct pairskt* skt = tkg (cls, caddr, port);
        push_q (q, (void*) skt);
        log_info (&logger, thrn, "Pushed a new task to the queue");
    }
}

int thread (void* arg)
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
    int sock, i, skl = sizeof (struct sockaddr_in), *ports;
    void* lib;
    int t;
    libinit init;
    thrd_t tid;
    char* tmu = strtmu ();
    log_init (&logger, tmu);
    free (tmu);
    memset (&v4, 0, sizeof (v4));
    struct in_addr in;
    switch (argc)
    {
        case 1:
            in.s_addr = htonl (INADDR_ANY);
            break;
        case 2:
            tmu = malloc (80);
            sscanf (argv [1], "%s", tmu);
            if (inet_aton (tmu, &in) == 0) goto INV_PAR;
            free (tmu);
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
    ports = init (&logger);
    q = new_q (LENQUEUE);
    for (i=0; i<THREAD_POOL_SIZE; i++)
    {
        if (thrd_create (&tid, thread, (void*) i))
        {
            log_fatal (&logger, "MAIN", "Failed to create thread!");
            return FAILED_CREATING_THREAD;
        }
        thread_pool [i] = tid;
    }
    for (i=0; ports [i]; i++)
    {
        tmu = malloc (sizeof (unsigned short) + sizeof (int));
        * (unsigned short*) tmu = ports [i];
        * (int*) (tmu + sizeof (unsigned short)) = i;
        if (thrd_create (&tid, listener, (void*) tmu))
        {
            log_fatal (&logger, "MAIN", "Failed to create listener thread!");
            return FAILED_CREATING_THREAD;
        }
        listeners [i] = tid;
    }
    thrd_join (listeners [0], &t);
    return 0;
    INV_PAR:
    log_fatal (&logger, "MAIN", "Something wrong happened while"
        " parsing arguments!");
    return INVALID_ARGUMENTS;
}
