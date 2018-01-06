#ifndef _HSYNCH_H_
#define _HSYNCH_H_

#include "ccprimitives.h"
#include "cctypes.h"
#include "clh.h"

//#define NUMA_AWARE

#ifdef NUMA_AWARE
	#define NCLUSTERS	getNumNodes()
#else
	#define NCLUSTERS	1
#endif

__thread int mycluster;

typedef struct HalfHSynchNode {
    struct HalfHSynchNode *next;
    RetVal (*fptr)(void*, ArgVal, int); /* DARKO BUGFIX */
    ArgVal arg_ret;
    int32_t pid;
    int32_t locked;
    int32_t completed;
} HalfHSynchNode;

typedef struct HSynchNode {
    struct HSynchNode *next;
    RetVal (*fptr)(void*, ArgVal, int); /* DARKO BUGFIX */
    ArgVal arg_ret;
    int32_t pid;
    int32_t locked;
    int32_t completed;
    int32_t align[PAD_CACHE(sizeof(HalfHSynchNode))];
} HSynchNode;

typedef struct HSynchThreadState {
    HSynchNode *next_node;
} HSynchThreadState;

typedef union HSynchNodePtr {
    struct HSynchNode *ptr;
    char pad[CACHE_LINE_SIZE];
} HSynchNodePtr;

typedef struct HSynchStruct {
    CLHLockStruct *central_lock CACHE_ALIGN;
    volatile HSynchNodePtr* Tail CACHE_ALIGN;
} HSynchStruct;

inline static RetVal applyOp(HSynchStruct *l, HSynchThreadState *st_thread, RetVal (*sfunc)(void *, ArgVal, int), void *state, ArgVal arg, int pid) {
    volatile HSynchNode *p;
    volatile HSynchNode *cur;
    register HSynchNode *next_node, *tmp_next;
    register int counter = -1; /* DARKO ADAPTATION */

    next_node = st_thread->next_node;
    next_node->next = null;
    next_node->locked = true;
    next_node->completed = false;

    cur = (volatile HSynchNode*)SWAP(&l->Tail[mycluster].ptr, next_node);
        
    cur->fptr = sfunc; /* DARKO BUGFIX*/
    cur->arg_ret = arg;
    cur->next = (HSynchNode *)next_node;
    st_thread->next_node = (HSynchNode *)cur;

    while (cur->locked) {                   // spinning
    	;
    }

    if (cur->completed)                     // I have been helped
        return cur->arg_ret;
    p = cur;                                // I have not been helped

	#ifdef NUMA_AWARE
    clhLock(l->central_lock, pid);
	#endif

    while (p->next != null && counter < MAX_COMBINED_OPS) {
        ReadPrefetch(p->next);
        counter++;
        tmp_next = p->next;
        //p->arg_ret = sfunc(state, p->arg_ret, p->pid);
        p->arg_ret = p->fptr(state, p->arg_ret, p->pid);	/* DARKO BUGFIX*/
        p->completed = true;
        p->locked = false;
        WeakFence();
        p = tmp_next;
    }
    p->locked = false;                      // Unlock the next one
    
    #ifdef NUMA_AWARE
    clhUnlock(l->central_lock, pid);
    #endif
    
    return cur->arg_ret;
}

inline static void HSynchThreadStateInit(HSynchThreadState *st_thread, int pid) {
    st_thread->next_node = getAlignedMemory(CACHE_LINE_SIZE, sizeof(HSynchNode));
    
    #ifdef NUMA_AWARE
    mycluster = getMyNode();
    #else
    mycluster = 0;
    #endif
}

void HSynchStructInit(HSynchStruct *l) {
    int i;

    l->central_lock = clhLockInit();
    l->Tail = getAlignedMemory(CACHE_LINE_SIZE, NCLUSTERS * sizeof(HSynchNodePtr));
    for (i = 0; i < NCLUSTERS; i++) {
        l->Tail[i].ptr = getAlignedMemory(CACHE_LINE_SIZE, sizeof(HSynchNode));
        l->Tail[i].ptr->next = null;
        l->Tail[i].ptr->locked = false;
        l->Tail[i].ptr->completed = false;
    }
    StoreFence();
}

#endif
