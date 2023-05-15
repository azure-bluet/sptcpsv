#include <pthread.h>
#include <stdlib.h>

typedef struct
{
    void** objs;
    int len;
    int start, end;
    pthread_mutex_t lock, objc, pslc;
} queue;

typedef void (*destruct) (void*);

queue* new_q (int len)
{
    queue* q = malloc (sizeof (queue));
    q -> len = len;
    q -> start = q -> end = 0;
    pthread_mutex_init (& (q -> lock), NULL);
    pthread_mutex_init (& (q -> objc), NULL);
    pthread_mutex_init (& (q -> pslc), NULL);
    q -> objs = calloc (len, sizeof (void*));
    return q;
}

int push_q (queue* q, void* v)
{
    int r = -1;
    pthread_mutex_lock (& (q -> pslc));
    if (q -> start == (q -> end + 1) % q -> len) goto PUSH_RET;
    r = 0;
    q -> objs [q -> end ++] = v;
    q -> end %= q -> len;
    pthread_mutex_unlock (& (q -> objc));
    pthread_mutex_unlock (& (q -> pslc));
    PUSH_RET:
    return r;
}

void* pop_q (queue* q)
{
    void* r = NULL;
    pthread_mutex_lock (& (q -> lock));
    if (q -> start == q -> end) pthread_mutex_lock (& (q -> objc));
    r = q -> objs [q -> start ++];
    q -> start %= q -> len;
    POP_RET:
    pthread_mutex_unlock (& (q -> lock));
    return r;
}

void del_q (queue* q, destruct d)
{
    int i, e, s;
    e = (q -> start > q -> end ? q -> len : q -> end);
    s = (q -> start > q -> end ? 0 : q -> end);
    for (i=q->start; i<e; i++) d (q -> objs [i]);
    for (i=s; i<q->end; i++) d (q -> objs [i]);
    pthread_mutex_destroy (& (q -> lock));
    pthread_mutex_destroy (& (q -> objc));
    pthread_mutex_destroy (& (q -> pslc));
    free (q);
}

#ifdef MIAN
#include <stdio.h>
void f (void*) {}
int main (void)
{
    queue* q;
    q = new_q (100);
    push_q (q, (void*) 123);
    push_q (q, (void*) 120975);
    printf ("%d\n", pop_q (q));
    printf ("%d\n", pop_q (q));
    del_q (q, f);
    printf ("awa\n");
    return 0;
}
#endif
