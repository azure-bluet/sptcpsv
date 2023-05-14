#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sptcpsv.h>
#include <logfile.h>
#include <unistd.h>

struct Logger* loggg;

void init (struct Logger* l) { loggg = l; }

struct pairskt* get_token (int sf, struct sockaddr_in s)
{
    char* lgn = malloc (120);
    sprintf (lgn, "Client connection: %s FINISHED", inet_ntoa (s.sin_addr));
    send (sf, "A!!", 3, 0);
    struct pairskt* skt = malloc (sizeof (struct pairskt));
    skt -> sockfd = sf;
    skt -> token = lgn;
    return skt;
}

struct pairskt* handle (struct pairskt* skt, int thrn, char* th)
{
    sleep (5);
    send (skt -> sockfd, "B??", 3, 0);
    close (skt -> sockfd);
    log_info (loggg, th, (char*) (skt -> token));
    free (skt -> token);
    free (skt);
    return NULL;
}
