#include <openssl/ssl3.h>
#include <openssl/tls1.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef UDBG
#define U(a) printf (#a " result %d at line %d\n", a, __LINE__)
#else
#define U(a) a
#endif

#ifdef awdebg
#define debg printf ("DEBUG %s %d\n", __FILE__, __LINE__);
#else
#define debg
#endif

int main (int argc, char** argv)
{
    unsigned short pt;
    sscanf (argv [1], "%hu", &pt);
    #if OPENSSL_VERSION_NUMBER < 0x1010001fL
    SSL_load_error_strings();
    SSL_library_init();
    #else
    OPENSSL_init_ssl (OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
    #endif
    SSL_CTX* ctx = SSL_CTX_new (TLS_method ());
    SSL_CTX_set_options (ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3
        | SSL_OP_NO_COMPRESSION | SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
    SSL* ssl = SSL_new (ctx);
    int sock = socket (AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in v;
    v.sin_addr.s_addr = INADDR_ANY;
    v.sin_port = htons (pt);
    v.sin_family = AF_INET;
    connect (sock, &v, sizeof (struct sockaddr_in));
    U (SSL_set_fd (ssl, sock));
    U (SSL_connect (ssl));
    char a [1050];
    U (SSL_read (ssl, a, 1024));
    printf (a);
    return 0;
}
