#include <stdlib.h>
#include <threads.h>

typedef struct
{
    void** objs;
    int len;
    int start, end;
    mtx_t lock, objc, pslc;
    cnd_t lcob;
} queue;

typedef void (*destruct) (void*);

queue* new_q (int len)
{
    queue* q = malloc (sizeof (queue));
    q -> len = len;
    q -> start = q -> end = 0;
    mtx_init (& (q -> lock), mtx_recursive);
    mtx_init (& (q -> objc), mtx_recursive);
    mtx_init (& (q -> pslc), mtx_recursive);
    cnd_init (& (q -> lcob));
    q -> objs = calloc (len, sizeof (void*));
    return q;
}

int push_q (queue* q, void* v)
{
    int r = -1;
    mtx_lock (& (q -> pslc));
    if (q -> start == (q -> end + 1) % q -> len) goto PUSH_RET;
    r = 0;
    q -> objs [q -> end ++] = v;
    q -> end %= q -> len;
    cnd_broadcast (& (q -> lcob));
    PUSH_RET:
    mtx_unlock (& (q -> pslc));
    return r;
}

void* pop_q (queue* q)
{
    void* r = NULL;
    mtx_lock (& (q -> lock));
    if (q -> start == q -> end)
    {
        mtx_lock (& (q -> objc));
        cnd_wait (& (q -> lcob), & (q -> objc));
        mtx_unlock (& (q -> objc));
    }
    r = q -> objs [q -> start ++];
    q -> start %= q -> len;
    mtx_unlock (& (q -> lock));
    return r;
}

void del_q (queue* q, destruct d)
{
    int i, e, s;
    e = (q -> start > q -> end ? q -> len : q -> end);
    s = (q -> start > q -> end ? 0 : q -> end);
    for (i=q->start; i<e; i++) d (q -> objs [i]);
    for (i=s; i<q->end; i++) d (q -> objs [i]);
    mtx_destroy (& (q -> lock));
    mtx_destroy (& (q -> objc));
    mtx_destroy (& (q -> pslc));
    cnd_destroy (& (q -> lcob));
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
