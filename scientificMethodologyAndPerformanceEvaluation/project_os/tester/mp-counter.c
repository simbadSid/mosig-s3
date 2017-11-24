#include <stdio.h>
#include "mp-server.h"
#include "main.h"
#include "ccrand.h"

#define SERVER_ID 0

volatile int64_t* counter;
__thread int64_t local_impact = 0;
int64_t check = 0;

#ifdef EMPTY_TX_DEBUG
__thread int steals = 0;
int allsteals = 0;
#endif

#ifdef LONGCS
#define ARRSIZE 64
static volatile uint64_t* array;
#endif

int64_t inc(int64_t arg, int unused) {
	#ifndef LONGCS
	(*counter)+=arg;
	return -arg;
	#else
	for(int i = 0; i < CS_LENGTH; i++)
		array[i%ARRSIZE]++;
	return -arg;
	#endif
}

void global_init(void)
{
	mpserver_global_init();
	#ifdef LONGCS
	posix_memalign((void**)&array, CLS, ARRSIZE*sizeof(uint64_t));
	#else
	posix_memalign((void**)&counter, CLS, CLS);
	*counter = 0;
	#endif
	
	if (nthreads < 2) {
		fprintf(stderr, "At least two threads!\n");
		exit(1);
	}
}

void thread_init(void)
{
	mpserver_thread_init();
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
	return mpserver_run_server();
}

void stop_server(int sid) {
	mpserver_stop_servers();
}

void user_iteration(int i)
{
	int64_t r = simRandomRange(1, 1000);
#ifndef EMPTY_TX_DEBUG
	if (mpserver_apply_op(SERVER_ID, inc, r) != (-r))
		fprintf(stderr, "oooops!!!\n");
#else
	steals += mpserver_apply_op(SERVER_ID, inc, r);
#endif
	local_impact += r;
}

void thread_cleanup() {
	__sync_fetch_and_add(&check, local_impact);
#ifdef EMPTY_TX_DEBUG
	__sync_fetch_and_add(&allsteals, steals);
#endif
}

void global_cleanup() {
#ifndef LONGCS
	if (*counter != check) { 
		fprintf(stderr, "ERROR! %lld : %lld\n", *counter, check);
	}
#endif
#ifdef EMPTY_TX_DEBUG
	printf("steals: %d\n", allsteals);
#endif
}
