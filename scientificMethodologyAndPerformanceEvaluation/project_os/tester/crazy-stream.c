#include <stdio.h>
#include <emmintrin.h>
#include "main.h"
#include "ccrand.h"
#include "mplib.h"
#include "cctypes.h"

#define SERVER_ID 0

typedef RetVal (*Fptr)(ArgVal, int);

static volatile int local_work = 1;
__thread int64_t counter = 0;
__thread int64_t failer;

int64_t globcnt = 0;

/*
int64_t inc(int64_t arg, int unused) {
	(*counter)+=arg;
	return arg;
}*/

void global_init(void)
{	
	mplib_global_init();
	
	if (nthreads < 2) {
		fprintf(stderr, "At least two threads!\n");
		exit(1);
	}
}

void thread_init(void)
{
	mplib_thread_init();
	//simSRandom(id);
}

bool is_server(int id)
{
	return id == SERVER_ID;
}

int server_count(void)
{
	return 1;
}

typedef union {
	__m128i qw;
	uint64_t ints[2];
} Jumbo;

int run_server(void) {
	int curr = 0;
	counter = 1;
	failer = 0;
	int64_t f;
	//__m128i qw;
	Jumbo j;

	while (local_work) {
		j.ints[0] = j.ints[1] = counter;
	
		//_mm_stream_si128((__m128i*)&mailboxes[id][curr]->int0, j.qw);
		//_mm_stream_si128((__m128i*)&mailboxes[id][curr]->int2, j.qw);
		//_mm_stream_si128((__m128i*)&mailboxes[id][curr]->int4, j.qw);
		_mm_stream_si128((__m128i*)&mailboxes[id][curr]->int6, j.qw);
		//_mm_mfence();
		
		counter++;
		
		//if (++curr >= nthreads) {
		//	curr = 0;
		//}
	}
	
	return 0;
}

void stop_server(int sid) {
	local_work = 0;
}

void user_iteration(int i)
{
	int64_t myflag, myintx;
	Jumbo j;
	volatile int64_t* flag = &(mailboxes[SERVER_ID][0]->flag);
	volatile int64_t* intx = &(mailboxes[SERVER_ID][0]->int6);
	
	while (main_work) {
		j.qw = (__m128i)_mm_load_ps((const float*)intx);
		_mm_mfence();
		myintx = j.ints[0];
		myflag = j.ints[1];
		if (myflag != myintx)
			printf("%lld vs %lld\n", myflag, myintx);
		//if (BACKOFF > 0)
		//	wait_cycles(BACKOFF);
	}
}

void thread_cleanup() {
}

void global_cleanup() {
	//printf("total client steals: %lld\n", globcnt);
	//printf("failer: %lld\n", failer);
}
