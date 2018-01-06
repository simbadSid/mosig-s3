#include <stdio.h>
#include <emmintrin.h>
#include "main.h"
#include "ccrand.h"
#include "mplib.h"
#include "cctypes.h"

#define SERVER_ID 0

uint64_t latency = 0;
int64_t globcnt = 0;
static volatile int local_work = 1;

volatile int64_t go CACHE_ALIGN = 0;
volatile int64_t ready CACHE_ALIGN = 0;

void global_init(void)
{	
	mplib_global_init();
	
	if (nthreads != 2) {
		fprintf(stderr, "Two threads!\n");
		exit(1);
	}
}

void thread_init(void)
{
	mplib_thread_init();
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

uint64_t counter;

int run_server(void) {
	//__m128i qw;
	Jumbo j;
	volatile CacheLine* my = mailboxes[id][0];
	my->flag=0xFFFF;
	counter = 0;

	while (local_work) {
		j.ints[0] = j.ints[1] = counter;
		go = 1;
		while(!ready && local_work) _mm_mfence();
		ready = 0;
		_mm_mfence();

		uint64_t time = get_cycle_count();
		_mm_stream_si128((__m128i*)&my->int0, j.qw);
		_mm_stream_si128((__m128i*)&my->int2, j.qw);
		_mm_stream_si128((__m128i*)&my->int4, j.qw);
		_mm_stream_si128((__m128i*)&my->int6, j.qw);
		//my->flag = counter;
		_mm_mfence();
		time = get_cycle_count() - time;
		counter++;
		latency += time;
	}
	latency /= counter;
	return 0;
}

void stop_server(int sid) {
	local_work = 0;
}

__thread int64_t loccnt = 0;

void user_iteration(int i)
{
	int64_t response;
	volatile int64_t* flag = &(mailboxes[SERVER_ID][0]->flag);
	
	//while (main_work) {
	//	response = *flag;
	//}

	while(!go && main_work) _mm_mfence();
	go = 0;
	//response = *flag;
	_mm_mfence();
	ready = 1;
	while((*flag)!=loccnt);
	asm volatile("clflush (%0)" :: "r" (flag));
	loccnt++;
}

void thread_cleanup() {
}

void global_cleanup() {
	printf("samples: %lld\n", counter);
	printf("latency: %lld\n", latency);
}
