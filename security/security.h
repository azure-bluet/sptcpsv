#ifndef SPTCPSVSECURITYH
#define SPTCPSVSECURITYH

#include <openssl/ssl3.h>

#ifdef __cplusplus

namespace sptcpsv
{

#endif

#ifdef __cplusplus

extern "C"
{

#endif

int init_sptcpsv_ssl (const char* certfile, const char* prvkeyfile);
SSL* ssl_new_connection (int sock);
void ssl_close (SSL* ssl);

#ifdef __cplusplus

}};

#endif

#endif
