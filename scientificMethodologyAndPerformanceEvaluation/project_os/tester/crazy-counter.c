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
	simSRandom(id);
}

bool is_server(int id)
{
	return id == SERVER_ID;
}

int server_count(void)
{
	return 1;
}

int run_server(void) {
	int curr = 0;
	counter = 0;
	failer = 0;
	int64_t f;
	__m128i qw;

	while (local_work) {
		//if (mailboxes[id][curr]->flag==REQ) {
		//	mailboxes[id][curr]->flag = RESP;
		//}
		if(!__sync_bool_compare_and_swap(&mailboxes[id][curr]->flag, REQ, RESP))
			failer++;
		
	
		//f = mailboxes[id][curr]->flag;
		//_mm_mfence();
		//mailboxes[id][curr]->flag = 456;
		//_mm_mfence();
		//asm volatile("clflush (%0)" :: "r" (&mailboxes[id][curr]->flag));
		//scatter:
		//f = mailboxes[curr][id]->flag;		
		//mailboxes[curr][id]->flag = 456;
		//__builtin_ia32_movntq(&mailboxes[id][curr]->int6, 456);
		//_mm_stream_si128((__m128i*)&mailboxes[id][curr]->int0, qw);
		//_mm_stream_si128((__m128i*)&mailboxes[id][curr]->int2, qw);
		//_mm_stream_si128((__m128i*)&mailboxes[id][curr]->int4, qw);
		//_mm_stream_si128((__m128i*)&mailboxes[id][curr]->int6, qw);

		//if (__sync_bool_compare_and_swap(&mailboxes[id][curr]->flag, 123, 456))
		//if (__sync_fetch_and_add(&mailboxes[id][curr]->flag, 1) == 123)
		counter++;
		//else
		//	failer++;
		
		
		//curr = simRandomRange(1, nthreads-1);
		if (++curr >= nthreads) {
			curr = 0;
		}
	}
	
	return 0;
}

void stop_server(int sid) {
	local_work = 0;
}

void user_iteration(int i)
{
	int64_t response;
	__m128i qw;
	volatile int64_t* flag = &(mailboxes[SERVER_ID][id]->flag);
	//volatile int64_t* flag = &(mailboxes[id][SERVER_ID]->flag);
	
	*flag = REQ;
	_mm_mfence();
	//while (__sync_fetch_and_add(flag, 0) != RESP) wait_cycles(BACKOFF);
	do {
		wait_cycles(BACKOFF);
		_mm_mfence();
		uint64_t time = get_cycle_count();
		//response = __sync_fetch_and_add(flag, 0);
		response = *flag;
		_mm_mfence();
		time = get_cycle_count() - time;
		if (time > 200 && response != RESP)
			counter++;
	} while (response != RESP);
	return;
	
	while (main_work) {
		//response = *flag;
		//asm volatile("clflush (%0)" :: "r" (flag));
		//*flag = 123;
		//_mm_stream_si128((__m128i*)&mailboxes[SERVER_ID][id]->int0, qw);
		//_mm_stream_si128((__m128i*)&mailboxes[SERVER_ID][id]->int2, qw);
		//_mm_stream_si128((__m128i*)&mailboxes[SERVER_ID][id]->int4, qw);
		//_mm_stream_si128((__m128i*)&mailboxes[SERVER_ID][id]->int6, qw);
		counter++;
		if (BACKOFF > 0)
			wait_cycles(BACKOFF);
	}
}

void thread_cleanup() {
	if (id!=SERVER_ID)
	__sync_fetch_and_add(&globcnt, counter);
	else
		printf("failed %lld\n", failer);
	//if (id==SERVER_ID)
		//printf("local %lld: %lld\n", id, counter);
}

void global_cleanup() {
	printf("total client steals: %lld\n", globcnt);
	//printf("failer: %lld\n", failer);
}
