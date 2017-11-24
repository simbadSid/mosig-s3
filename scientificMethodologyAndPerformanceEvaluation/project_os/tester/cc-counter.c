#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "main.h"
#include "hsynch.h"
#include "ccrand.h"

volatile int64_t* counter CACHE_ALIGN;

HSynchStruct counter_lock CACHE_ALIGN;
__thread HSynchThreadState st_thread;

#ifdef LONGCS
#define ARRSIZE 64
static volatile uint64_t* array;
#endif

static RetVal fetchAndInc(void* state, ArgVal arg, int pid) {
#ifndef LONGCS
    int64_t ret = *counter;
    *counter += arg;
    return ret;
#else
	for(int i = 0; i < CS_LENGTH; i++)
		array[i%ARRSIZE]++;
	return arg;
#endif
}

void global_init(void)
{
	#ifdef LONGCS
	posix_memalign((void**)&array, CACHE_LINE_SIZE, ARRSIZE*sizeof(uint64_t));
	#else
	posix_memalign((void**)&counter, CACHE_LINE_SIZE, CACHE_LINE_SIZE);
	*counter = 0;
	#endif
    HSynchStructInit(&counter_lock);
}

void thread_init(void)
{
	HSynchThreadStateInit(&st_thread, (int)id);
}

void user_iteration(int i)
{
	applyOp(&counter_lock, &st_thread, fetchAndInc, null, 1, id);
}

bool is_server(int id) { return false; }

int server_count(void) { return 0; }

int run_server(void) { return 0; }

void stop_server(int id) {}

void thread_cleanup() {
	#ifdef GET_STATS
	__sync_fetch_and_add(&tot_requests, requests);
	__sync_fetch_and_add(&tot_instances, instances);
	#endif
}

void global_cleanup() {
	#ifdef GET_STATS
	printf("combining degree: %f\n", ((float)tot_requests)/tot_instances);
	#endif
}
