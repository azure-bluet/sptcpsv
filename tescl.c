#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define MAXLINE 4096
int main (int argc, char** argv)
{
    int sockfd, n;
    char sendline[4096];
    struct sockaddr_in servaddr;
    if (argc != 2)
    {
        printf ("usage: ./client <ipaddress>\n");
        exit (0);
    }
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf ("create socket error: %s(errno: %d)\n", strerror (errno), errno);
        exit (0);
    }
    memset (& servaddr, 0, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons (80);
    if (inet_pton (AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        printf ("inet_pton error for %s\n", argv [1]);
        exit (0);
    }
    if (connect (sockfd, (struct sockaddr*) &servaddr, sizeof (servaddr)) < 0)
    {
        printf ("connect error: %s(errno: %d)\n", strerror (errno), errno);
        exit (0);
    }
    int b = 0;
    printf ("RECV: %s\n\n", (b |= recv (sockfd, sendline, 4096, 0), sendline));
    send (sockfd, "R!!\n", 4, 0);
    printf ("RECV: %s\n\n", (b |= recv (sockfd, sendline, 4096, 0), sendline));
    printf ("%d\n\n", b);
    close (sockfd);
    exit (0);
}
