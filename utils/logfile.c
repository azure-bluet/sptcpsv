#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

#ifndef MAX_LOG_LINE
#define MAX_LOG_LINE 1024
#endif

#ifndef dbg
#define dbg printf ("Debug@%s %d\n", __FILE__, __LINE__)
#endif

#ifndef STDOUT
#define STDOUT STDOUT_FILENO
#endif

#ifndef STDERR
#define STDERR STDERR_FILENO
#endif

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

int log_init (struct Logger* logger, char* fn)
{
    int fd = open (fn, O_WRONLY | O_CREAT);
    if (fd == -1) return -1;
    pthread_mutex_init (&(logger -> lock), NULL);
    logger -> fd = fd;
    return 0;
}

int log_close (struct Logger* logger)
{ return close (logger -> fd); }

struct LogErrCode log_info (struct Logger* logger, char* name, char* str)
{
    if (strlen (name) + strlen (str) > MAX_LOG_LINE - 14 - tm_len)
        return (struct LogErrCode) {-1, -1};
    char content [MAX_LOG_LINE+1];
    struct LogErrCode ec;
    unsigned int lc;
    int a, b;
    sprintf (content, "[Info] [%s] [%s] %s\n", strtm (), name, str);
    lc = strlen (content);
    pthread_mutex_lock (& (logger -> lock));
    a = write (STDOUT, content, lc);
    b = write (logger -> fd, content, lc);
    pthread_mutex_unlock (& (logger -> lock));
    if (a == -1) ec.err_stdout = errno;
    else ec.err_stdout = 0;
    if (b == -1) ec.err_file = errno;
    else ec.err_file = 0;
    return ec;
}

struct LogErrCode log_warn (struct Logger* logger, char* name, char* str)
{
    if (strlen (name) + strlen (str) > MAX_LOG_LINE - 14 - tm_len)
        return (struct LogErrCode) {-1, -1};
    char content [MAX_LOG_LINE+1], scontent [MAX_LOG_LINE+9];
    struct LogErrCode ec;
    unsigned int lc;
    int a, b;
    sprintf (content, "[Warn] [%s] [%s] %s", strtm (), name, str);
    sprintf (scontent, "\e[33m%s\e[m\n", content);
    lc = strlen (content);
    content [lc++] = '\n';
    content [lc] = 0;
    pthread_mutex_lock (& (logger -> lock));
    a = write (STDOUT, scontent, lc + 8);
    b = write (logger -> fd, content, lc);
    pthread_mutex_unlock (& (logger -> lock));
    if (a == -1) ec.err_stdout = errno;
    else ec.err_stdout = 0;
    if (b == -1) ec.err_file = errno;
    else ec.err_file = 0;
    return ec;
}

struct LogErrCode log_error (struct Logger* logger, char* name, char* str)
{
    if (strlen (name) + strlen (str) > MAX_LOG_LINE - 15 - tm_len)
        return (struct LogErrCode) {-1, -1};
    char content [MAX_LOG_LINE+1], scontent [MAX_LOG_LINE+9];
    struct LogErrCode ec;
    unsigned int lc;
    int a, b;
    sprintf (content, "[ERROR] [%s] [%s] %s", strtm (), name, str);
    sprintf (scontent, "\e[31m%s\e[m\n", content);
    lc = strlen (content);
    content [lc++] = '\n';
    content [lc] = 0;
    pthread_mutex_lock (& (logger -> lock));
    a = write (STDERR, scontent, lc + 8);
    b = write (logger -> fd, content, lc);
    pthread_mutex_unlock (& (logger -> lock));
    if (a == -1) ec.err_stdout = errno;
    else ec.err_stdout = 0;
    if (b == -1) ec.err_file = errno;
    else ec.err_file = 0;
    return ec;
}

struct LogErrCode log_fatal (struct Logger* logger, char* name, char* str)
{
    if (strlen (name) + strlen (str) > MAX_LOG_LINE - 15 - tm_len)
        return (struct LogErrCode) {-1, -1};
    char content [MAX_LOG_LINE+1], scontent [MAX_LOG_LINE+13];
    struct LogErrCode ec;
    unsigned int lc;
    int a, b;
    sprintf (content, "[FATAL] [%s] [%s] %s", strtm (), name, str);
    sprintf (scontent, "\e[31;1;5m%s\e[m\n", content);
    lc = strlen (content);
    content [lc++] = '\n';
    content [lc] = 0;
    pthread_mutex_lock (& (logger -> lock));
    a = write (STDERR, scontent, lc + 10);
    b = write (logger -> fd, content, lc);
    pthread_mutex_unlock (& (logger -> lock));
    if (a == -1) ec.err_stdout = errno;
    else ec.err_stdout = 0;
    if (b == -1) ec.err_file = errno;
    else ec.err_file = 0;
    return ec;
}

#ifdef MAIN_LOG
int main (void)
{
    struct Logger l;
    log_init (&l, "awe");
    log_info (&l, "awe", "AWAWA");
    log_warn (&l, "awe", "WAWAW");
    log_error (&l, "awe", "WTF");
    log_fatal (&l, "awe", "WWWWWTF");
    log_close (&l);
    return 0;
}
#endif

