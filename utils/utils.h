#ifndef SPTCPSV_UTILS
#define SPTCPSV_UTILS

#define tm_len 24
#define tmu_len 20

extern char* strtm (void);
extern char* strtmu (void); /* You should free the result of strtmu! */

#endif
