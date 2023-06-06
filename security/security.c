#include <openssl/ssl3.h>
#include <openssl/tls1.h>
#include <stdlib.h>
#include <sys/socket.h>

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

SSL_CTX* sptcpsv_ctx;

int init_sptcpsv_ssl (const char* certfile, const char* prvkeyfile)
{
    #if OPENSSL_VERSION_NUMBER < 0x1010001fL
    SSL_load_error_strings();
    SSL_library_init();
    #else
    OPENSSL_init_ssl (OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
    #endif
    sptcpsv_ctx = SSL_CTX_new (TLS_method ());
    if (sptcpsv_ctx == NULL) return 1;
    SSL_CTX_set_options (sptcpsv_ctx, SSL_OP_ALL
        | SSL_OP_NO_COMPRESSION | SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
    if (SSL_CTX_use_certificate_file (sptcpsv_ctx, certfile, SSL_FILETYPE_PEM) != 1) return 2;
    if (SSL_CTX_use_PrivateKey_file (sptcpsv_ctx, prvkeyfile, SSL_FILETYPE_PEM) != 1) return 2;
}

SSL* ssl_new_connection (int sock)
{
    SSL* n = SSL_new (sptcpsv_ctx);
    if (n == NULL) return n;
    U (SSL_set_fd (n, sock));
    U (SSL_get_error (n, SSL_accept (n)));
    return n;
}

void ssl_close (SSL* ssl)
{
    SSL_shutdown (ssl);
    SSL_free (ssl);
}

#ifdef MIAM

#include <netinet/in.h>

int main (int argc, char** argv)
{
    unsigned short pt;
    sscanf (argv [1], "%hu", &pt);
    U (init_sptcpsv_ssl ("escalopia_net.pem", "escalopia_net_private.pem"));
    /* NOTE: I tested so much and couldn't find out that i did not put the pem files here */
    int sock = socket (AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in v;
    v.sin_addr.s_addr = INADDR_ANY;
    v.sin_port = htons (pt);
    v.sin_family = AF_INET;
    U (bind (sock, &v, sizeof (struct sockaddr)));
    U (listen (sock, 100));
    int cd = accept (sock, NULL, NULL);
    SSL* ssl = ssl_new_connection (cd);
    U (SSL_write (ssl, "Hello, SSL/TLS!\n\n\n", 18));
    ssl_close (ssl);
    close (cd);
    close (sock);
    return 0;
}

#endif

