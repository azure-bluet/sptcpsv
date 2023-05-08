#ifndef SPTCPSV_LOGFILE
#define SPTCPSV_LOGFILE

#include <pthread.h>

struct Logger
{
    pthread_mutex_t lock;
    int fd;
};
struct LogErrCode
{
    int err_stdout;
    int err_file;
};

extern int log_init (struct Logger*, char*);
extern int log_close (struct Logger*);
extern struct LogErrCode log_info (struct Logger*, char*, char*);
extern struct LogErrCode log_warn (struct Logger*, char*, char*);
extern struct LogErrCode log_error (struct Logger*, char*, char*);
extern struct LogErrCode log_fatal (struct Logger*, char*, char*);

#endif
