#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <numa.h>
#include "main.h"
#include "ccrand.h"

//////////////////
//defined through command line
//#define ALIGNMENT 64
//#define STREAMING
//#define WREAD
//#define BACKOFF 2000
/////////////////

//#define SHUFFLE

#define REQ 1
#define RESP 2
#define EMPTY 0

#if defined(BACKOFF) && BACKOFF>0
	#define PAUSE wait_cycles(BACKOFF);
#else
	#define PAUSE __asm__ __volatile__("pause;")
#endif

#ifdef WREAD
	#define READ(a) __sync_fetch_and_add(&a, 0)
	//#define READ(a) (__builtin_prefetch(&a, 1, 3), a)
#else
	#define READ(a) a
#endif

#if defined(STREAMING) || defined(EMPTY_TX_DEBUG)
#include <emmintrin.h>

typedef union {
	__m128i qw;
	int64_t w[2];
} uni;
#endif

typedef struct cacheline {
	int64_t int0, int1, int2, int3, int4, int5, int6;
	int64_t flag;
} CacheLine;

#define CLS (sizeof(CacheLine))

volatile CacheLine*** mailboxes;

void mplib_global_init() {
	volatile CacheLine** matrix;
	//matrix = malloc(nthreads*nthreads*sizeof(CacheLine*));
	posix_memalign((void**)&matrix, 4096, nthreads*nthreads*sizeof(CacheLine*));
	memset(matrix, 0, nthreads * nthreads * sizeof(CacheLine*));
	
	//mailboxes = malloc(nthreads*sizeof(CacheLine**));
	posix_memalign((void**)&mailboxes, 4096, nthreads*sizeof(CacheLine**));
	for (int i = 0; i < nthreads; i++)
		mailboxes[i] = &matrix[i*nthreads];	
}

void mplib_thread_init() {

	CacheLine* lines;


	posix_memalign((void**)&lines, ALIGNMENT, nthreads * ALIGNMENT);
	//lines = numa_alloc_local(nthreads * ALIGNMENT);
	//lines = malloc(nthreads * ALIGNMENT);
	memset(lines, 0, nthreads * ALIGNMENT);	

#ifndef SHUFFLE	
	for (int i = 0; i < nthreads; i++) {
		mailboxes[id][i] = &lines[(ALIGNMENT/CLS)*i];
		mailboxes[id][i]->flag = EMPTY;
	}
#else
	for (int i = 0; i < nthreads; i++) {
		lines[(ALIGNMENT/CLS)*i].flag = REQ;
	}
	
	for (int i = 0; i < nthreads; i++) {
		int curr;
		do {
		 curr = simRandomRange(0, nthreads);
		} while (lines[(ALIGNMENT/CLS)*curr].flag != REQ);
		mailboxes[id][i] = &lines[(ALIGNMENT/CLS)*curr];
		mailboxes[id][i]->flag = EMPTY;
	}
#endif
}

inline void mplib_put2(int dest, int offset, int64_t flag, int64_t int0, int64_t int1) {
	volatile CacheLine* t = mailboxes[dest][offset];
	
	#ifdef STREAMING
	uni a;
	a.w[0] = int0;
	a.w[1] = int1;
	_mm_stream_si128((__m128i*)&t->int0, a.qw);
	_mm_stream_si128((__m128i*)&t->int2, a.qw);
	_mm_stream_si128((__m128i*)&t->int4, a.qw);
	a.w[1] = flag;
	_mm_stream_si128((__m128i*)&t->int6, a.qw);
	#else
	t->int0 = int0;
	t->int1 = int1;
	t->int2 = int1;
	t->int3 = int1;
	t->int4 = int1;
	t->int5 = int1;
	t->int6 = int1;
	t->flag = flag;
	#endif
}

inline void mplib_put1(int dest, int offset, int64_t flag, int64_t int0) {
	volatile CacheLine* t = mailboxes[dest][offset];
	
	#ifdef STREAMING
	uni a;
	a.w[0] = int0;
	a.w[1] = flag;
	_mm_stream_si128((__m128i*)&t->int0, a.qw);
	_mm_stream_si128((__m128i*)&t->int2, a.qw);
	_mm_stream_si128((__m128i*)&t->int4, a.qw);
	_mm_stream_si128((__m128i*)&t->int6, a.qw);
	#else
	t->int0 = int0;
	t->int1 = int0;
	t->int2 = int0;
	t->int3 = int0;
	t->int4 = int0;
	t->int5 = int0;
	t->int6 = int0;
	t->flag = flag;
	#endif
}

inline void mplib_get2(int dest, int offset, int64_t flag, int64_t* int0, int64_t* int1) {
	volatile CacheLine* t = mailboxes[dest][offset];
	while (READ(t->flag) != flag) PAUSE;
	*int0 = t->int0;
	*int1 = t->int1;
}

#ifndef EMPTY_TX_DEBUG
inline void mplib_get1(int dest, int offset, int64_t flag, int64_t* int0) {
	volatile CacheLine* t = mailboxes[dest][offset];
	while (READ(t->flag) != flag) PAUSE;
	//while(__sync_fetch_and_add(&t->flag, 0) != flag) PAUSE;
	*int0 = t->int0;
}
#else

inline int mplib_get1(int dest, int offset, int64_t flag, int64_t* int0) {
	volatile CacheLine* t = mailboxes[dest][offset];
	int64_t response;
	int steals = 0;
	do {
		_mm_mfence();
		//wait_cycles(BACKOFF);
		uint64_t time = get_cycle_count();
		response = READ(t->flag);
		_mm_mfence();
		time = get_cycle_count() - time;
		if (time > 300 && response != RESP) {
			steals++;
		}
	} while (response != RESP);

	*int0 = t->int0;
	return steals;
}
#endif


inline int mplib_try_get2(int dest, int offset, int64_t flag, int64_t* int0, int64_t* int1) {
	int ret = 0;
	volatile CacheLine* t = mailboxes[dest][offset];
	if (READ(t->flag) == flag) {
		*int0 = t->int0;
		*int1 = t->int1;
		ret = 1;
	}
	return ret;
}

inline int mplib_try_get1(int dest, int offset, int64_t flag, int64_t* int0) {
	int ret = 0;
	volatile CacheLine* t = mailboxes[dest][offset];
	if (READ(t->flag) == flag) {
		*int0 = t->int0;
		ret = 1;
	}
	return ret;
}

inline void mplib_send1(int dest, int64_t int0) {
	mplib_put1(dest, id, REQ, int0);
}

inline void mplib_send2(int dest, int64_t int0, int64_t int1) {
	mplib_put2(dest, id, REQ, int0, int1);
}

inline void mplib_recv1(int src, int64_t* int0) {
	//__builtin_prefetch(mailboxes[id][src], 1, 3);
	mplib_get1(id, src, REQ, int0);
	mailboxes[id][src]->flag = EMPTY;
}

inline void mplib_recv2(int src, int64_t* int0, int64_t* int1) {
	//__builtin_prefetch(mailboxes[id][src], 1, 3);
	mplib_get2(id, src, REQ, int0, int1);
	mailboxes[id][src]->flag = EMPTY;
}

inline int mplib_try_recv1(int src, int64_t* int0) {
	int ret;
	if(ret = mplib_try_get1(id, src, REQ, int0)) {
		mailboxes[id][src]->flag = EMPTY;
	}
	return ret;
}

inline int mplib_try_recv2(int src, int64_t* int0, int64_t* int1) {
	int ret;
	if(ret = mplib_try_get2(id, src, REQ, int0, int1)) {
		mailboxes[id][src]->flag = EMPTY;
	}
	return ret;
}
