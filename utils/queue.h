#ifndef SPTCPSV_QUEUE
#define SPTCPSV_QUEUE
#include <pthread.h>

typedef struct _sptcpsv_queue queue;
typedef void (*queue_obj_destruct) (void*);
queue* new_q (int);
int push_q (queue*, void* v);
void* pop_q (queue*);
void del_q (queue*, queue_obj_destruct);

#endif
