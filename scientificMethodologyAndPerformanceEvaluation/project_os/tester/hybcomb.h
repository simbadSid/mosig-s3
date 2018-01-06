#include <sys/types.h>
#include <malloc.h>
#include <stdbool.h>
#include "mplib.h"
#include "ccprimitives.h"
#include "clh.h"

//#define NUMA_AWARE

#ifdef NUMA_AWARE
	#define NCLUSTERS getNumNodes()
#else
	#define NCLUSTERS 1
#endif

//#define FIXED_COMBINER
//#define GET_STATS
//#define INLINE_CS

#ifdef FIXED_COMBINER
	#define MAX_COMBINED_OPS (1 << 30)
#endif
/* else defined in the makefile */

#define DUMMY 0xFFFF

#ifdef GET_STATS
static __thread int instances = 0;
static __thread int requests = 0;
static __thread int cas_exec = 0;
static __thread int cas_succ = 0;

static int tot_instances = 0;
static int tot_requests = 0;
static int tot_cas_exec = 0;
static int tot_cas_succ = 0;
#endif

typedef RetVal (*Fptr)(volatile void*, ArgVal);

typedef struct node {
	int64_t n_ops CACHE_ALIGN;
	int thread_id CACHE_ALIGN;
	bool next_wait CACHE_ALIGN;
} CombiningNode;

typedef struct hyblock {
	//volatile CombiningNode* head CACHE_ALIGN;
	volatile CombiningNode** heads CACHE_ALIGN;
	volatile CombiningNode** dummies CACHE_ALIGN;
	volatile void* concurrent_object;
	CLHLockStruct *central_lock CACHE_ALIGN;
} HybLock;

/* FIXME should be defined differently */
__thread volatile CombiningNode* my_node;

__thread int mycluster;

void hybcomb_init(volatile HybLock* lock, volatile void* object) {	
	mplib_global_init();
	//lock->head = getAlignedMemory(CACHE_LINE_SIZE, sizeof(CombiningNode));
	lock->heads = getAlignedMemory(CACHE_LINE_SIZE, NCLUSTERS * sizeof(CombiningNode*));
	lock->dummies = getAlignedMemory(CACHE_LINE_SIZE, NCLUSTERS * sizeof(CombiningNode*));
	
	for (int i = 0; i < NCLUSTERS; i++) {
		lock->heads[i] = getAlignedMemory(CACHE_LINE_SIZE, sizeof(CombiningNode));
		lock->dummies[i] = (CombiningNode*)lock->heads[i];
		lock->dummies[i]->thread_id = DUMMY;
		lock->dummies[i]->n_ops = MAX_COMBINED_OPS;
		lock->dummies[i]->next_wait = false;
	}
	
	lock->concurrent_object = object;
	
	lock->central_lock = clhLockInit();
}

void hybcomb_thread_init() {
	mplib_thread_init();
	my_node = getAlignedMemory(CACHE_LINE_SIZE, sizeof(CombiningNode));
	my_node->thread_id = id;
	my_node->n_ops = MAX_COMBINED_OPS;
	my_node->next_wait = true;
	
	#ifdef NUMA_AWARE
	mycluster = getMyNode();
	#else
	mycluster = 0;
	#endif
}

#ifdef INLINE_CS
__attribute__((always_inline)) inline void* inline_cs(volatile void* obj, void* arg);
#endif

/*
static inline void treat_request(volatile void* object) {

	int sender_id;
	Fptr fptr;
	void* retval, *fargs;
	
	sender_id = tmc_udn0_receive();
	fptr = (Fptr)tmc_udn0_receive();
	fargs = (void*)tmc_udn0_receive();

#ifdef INLINE_CS
	retval = inline_cs(object, fargs); 
#else
	retval = fptr(object, fargs);
#endif
	
	tmc_udn_send_1(headers[sender_id], UDN0_DEMUX_TAG, (uint_reg_t)retval);
}
*/

RetVal apply_op(HybLock* lock, Fptr fptr, ArgVal fargs) {
	CombiningNode* head;
	int ops_completed = 0;
	int* ptr;
	ArgVal args;
	RetVal retval;
	int sender_id;
	int64_t finops;
	volatile void* object = lock->concurrent_object;

	while (1) {	
		head = (CombiningNode*)lock->heads[mycluster];
		if (FAA64(&(head->n_ops), 1LL) < MAX_COMBINED_OPS) {
			/* send request to head->core_id  and wait for response*/
			mplib_put2(head->thread_id, id, REQ, (int64_t)fptr, (int64_t)fargs);
			mplib_get1(head->thread_id, id, RESP, (int64_t*)(&retval));
			return retval;
		} else {
			/* try to register as (next) combiner */
			if (CAS64(&(lock->heads[mycluster]), head, my_node)) {
				#ifdef GET_STATS
				cas_exec++; cas_succ++;
				#endif
				my_node->n_ops = 0;
				while (head->next_wait) __sync_synchronize ();
				break;
			}
			#ifdef GET_STATS
			cas_exec++;
			#endif
		}
	}
	
	#ifdef NUMA_AWARE
    clhLock(lock->central_lock, id);
	#endif

	/* run your own req */
	#ifdef INLINE_CS
	retval = inline_cs(object, fargs);
	#else
	retval = fptr(object, fargs);
	#endif
	
	/* now run through the reqs and handle them */
	#ifdef FIXED_COMBINER
	volatile register int loctmout asm("r15") = 0;
	while (1) {
		if (tmc_udn0_available_count() > 0) {
			treat_request(object);
			ops_completed++;
			loctmout = 0;
		} else if (loctmout++ > 50000) break;
	}
	#else
	
	/* now act as an mp-server, until you fail for FORGIVE consecutive times */
	int curr = 0;
	Fptr func;
	ArgVal arg;
	RetVal res;
	int forgive = 5000;

	while (forgive) {
		if(mplib_try_get2(id, curr, REQ, (int64_t*)(&func), (int64_t*)(&arg))) {
			res = func(object, arg);
			mplib_put1(id, curr, RESP, (int64_t)res);
			ops_completed++;
			forgive = 5000;
		} else
			forgive--;
		if (++curr >= nthreads) {
			curr = 0;
		}
	}	
	#endif
	
	/* close the combiner */
	finops = (int64_t)SWAP(&(my_node->n_ops), MAX_COMBINED_OPS);
	//__sync_synchronize();
	if (finops > MAX_COMBINED_OPS) {
		finops = MAX_COMBINED_OPS;
	}
	/* see if there are "late requests" and handle them */
	while (ops_completed < finops) {
		if(mplib_try_get2(id, curr, REQ, (int64_t*)(&func), (int64_t*)(&arg))) {
			res = func(object, arg);
			mplib_put1(id, curr, RESP, (int64_t)res);
			ops_completed++;
		}
		if (++curr >= nthreads) {
			curr = 0;
		}
	}
	
	#ifdef NUMA_AWARE
    clhUnlock(lock->central_lock, id);
	#endif
	
	/* exchange node and inform next */
	/* head only used as tmp */
	head = (CombiningNode*)my_node;
	my_node = lock->dummies[mycluster];
	lock->dummies[mycluster] = head;
	
	/* note: unlike Tilera, no fences needed here as we have TSO */
	my_node->next_wait = true;
	my_node->thread_id = lock->dummies[mycluster]->thread_id;
	lock->dummies[mycluster]->next_wait = false;

	#ifdef GET_STATS
	instances++;
	requests += ops_completed;
	#endif
	
	return retval;
}
