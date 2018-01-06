#ifndef _CLH_H_

#define _CLH_H_

#include <stdlib.h>
#include <pthread.h>

#include "ccprimitives.h"

#ifdef POSIX_LOCKS

typedef pthread_mutex_t CLHLockStruct;

inline static void clhLock(LockStruct *l, int pid) {
    pthread_mutex_lock(l);
}

inline static void clhUnlock(LockStruct *l, int pid) {
    pthread_mutex_unlock(l);
}

LockStruct *clhLockInit(void) {
    LockStruct *l, tmp = PTHREAD_MUTEX_INITIALIZER;
    int error;

    error = posix_memalign((void *)&l, CACHE_LINE_SIZE, sizeof(CLHLockStruct));
    *l = tmp;
    return l;
}

#else

#define N_THREADS 200

typedef union CLHLockNode {
    bool locked;
    char align[CACHE_LINE_SIZE];
} CLHLockNode;

typedef struct CLHLockStruct {
    volatile CLHLockNode *Tail CACHE_ALIGN;
    volatile CLHLockNode *MyNode[N_THREADS] CACHE_ALIGN;
    volatile CLHLockNode *MyPred[N_THREADS] CACHE_ALIGN;
} CLHLockStruct;


inline static void clhLock(CLHLockStruct *l, int pid) {
    l->MyNode[pid]->locked = true;
    l->MyPred[pid] = (CLHLockNode *)SWAP(&l->Tail, (void *)l->MyNode[pid]);
    while (l->MyPred[pid]->locked == true) {
    }
}

inline static void clhUnlock(CLHLockStruct *l, int pid) {
    l->MyNode[pid]->locked = false;
    l->MyNode[pid]= l->MyPred[pid];
#ifdef sparc
    StoreFence();
#endif
}

CLHLockStruct *clhLockInit(void) {
    CLHLockStruct *l;
    int j;

    l = getAlignedMemory(CACHE_LINE_SIZE, sizeof(CLHLockStruct));
    l->Tail = getAlignedMemory(CACHE_LINE_SIZE, sizeof(CLHLockNode));
    l->Tail->locked = false;

    for (j = 0; j < N_THREADS; j++) {
        l->MyNode[j] = getAlignedMemory(CACHE_LINE_SIZE, sizeof(CLHLockNode));
        l->MyPred[j] = null;
    }

    return l;
}
#endif

#endif
