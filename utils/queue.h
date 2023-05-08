#ifndef SPTCPSV_QUEUE
#define SPTCPSV_QUEUE

#include <pthread.h>
#include <stdlib.h>

struct queue
{
    void** objs;
    int len;
    int start, end;
    pthread_mutex_t lock;
};

inline int push_q (struct queue q, void* v)
{
    pthread_mutex_lock (&(q.lock));
    if (q.start == (q.end + 1) % q.len) return -1;
    q.objs [q.end ++] = v;
    q.end %= q.len;
    pthread_mutex_unlock (&(q.lock));
    return 0;
}

inline void* pop_q (struct queue q)
{
    pthread_mutex_lock (&(q.lock));
    if (q.start == q.end) return NULL;
    void* r = q.objs [q.start ++];
    q.start %= q.len;
    pthread_mutex_unlock (&(q.lock));
    return r;
}

#endif
